/*
 *  ofxiVideoLoader.cpp
 *  VideoToTextures
 *
 *  Created by Roee Kremer on 12/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiVideoLoader.h"

#import <AVFoundation/AVFoundation.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#define SINT16_MAX 32767.0

void ofxiVideoLoader::setup(ofxiPhoneVideo *video) {
	this->video = video;
	fbo.setup();
}


GLuint createVideoTextue(GLuint w,GLuint h)
{
	int dataSize = w * h * 4;
	uint8_t* textureData = (uint8_t*)malloc(dataSize);
	if(textureData == NULL)
		return 0;
	memset(textureData, 128, dataSize);
	
	GLuint handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, 
				 GL_UNSIGNED_BYTE, textureData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(textureData);
	
	return handle;
}


void renderCameraToSprite(GLuint text,float w,float h)
{
	if (!text)
		return;
	
	GLfloat spriteTexcoords[] = {
		1.0f,1.0f,   
		1.0f,0.0f,
		0,1.0f,   
		0.0f,0,};
	
	
	
	
	GLfloat spriteVertices[] =  {
		w,h,0,   
		w,0,0,   
		0,h,0, 
		0,0,0};
	
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
	glBindTexture(GL_TEXTURE_2D, text);
	glEnable(GL_TEXTURE_2D);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	//	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	//	glEnable(GL_DEPTH_TEST);
	
	glDisable(GL_TEXTURE_2D);
	
}

void ofxiVideoLoader::load(string filename) {
	
	
	BOOL test;
	
	AVURLAsset *asset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:[NSString stringWithFormat:@"%s",filename.c_str()]] options:nil];
	
	NSError *error = nil;
	AVAssetReader *reader = [AVAssetReader assetReaderWithAsset:asset error:&error];
	
	if (error) {
		NSLog(@"AVAssetReader: %@",[error description]);
	}
	
	NSArray *videoTracks = [asset tracksWithMediaType:AVMediaTypeVideo];
	
	NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey, 
										   [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],(NSString*)kCVPixelBufferPixelFormatTypeKey,
										   /*[NSNumber numberWithUnsignedInt:size.width],(NSString*)kCVPixelBufferWidthKey,
											[NSNumber numberWithUnsignedInt:size.height],(NSString*)kCVPixelBufferHeightKey,*/nil];
	
	
	AVAssetReaderTrackOutput * videoOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[videoTracks objectAtIndex:0] outputSettings:pixelBufferAttributes];
	NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[videoOutput mediaType]);
	
		
	 test = [reader canAddOutput:videoOutput];
	NSLog(@"can add output: %i",test);
	[reader addOutput:videoOutput];
	
	
	
	NSArray *audioTracks = [asset tracksWithMediaType:AVMediaTypeAudio];
	NSDictionary* audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM ], AVFormatIDKey,nil];
	
	AVAssetReaderTrackOutput * audioOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[audioTracks objectAtIndex:0] outputSettings:audioOutputSettings];
	NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[audioOutput mediaType]);
	
		
	test = [reader canAddOutput:audioOutput];
	NSLog(@"can add output: %i",test);
	[reader addOutput:audioOutput];
	
	
	
	
	
	//	EAGLContext* contextB = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:contextA.sharegroup];
	//	[EAGLContext setCurrentContext:contextB];
	
	[reader startReading];
	
	bool bVideoDone = false;
	bool bAudioDone = false;
	
	video->textures.clear();
	
	CMVideoDimensions videoDimensions;
	CMVideoCodecType videoType;
	float *buffer = new float[video->sampleRate * 2]; // max 2 sec
	int pos = 0;
	fbo.push(video->textureWidth, video->textureHeight);
	GLuint videoTexture;
	float renderWidth;
	float renderHeight;
	
	while (reader.status == AVAssetReaderStatusReading) {
		
		if (!bAudioDone) {
			CMSampleBufferRef  sampleBuffer = [audioOutput copyNextSampleBuffer];
			if (sampleBuffer == NULL) {
				bAudioDone = true;
			} else
			{
				CMItemCount numSamples = CMSampleBufferGetNumSamples(sampleBuffer);
				NSUInteger channelIndex = 0;
				
				CMBlockBufferRef audioBlockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
				size_t audioBlockBufferOffset = (channelIndex * numSamples * sizeof(SInt16));
				size_t lengthAtOffset = 0;
				size_t totalLength = 0;
				SInt16 *samples = NULL;
				CMBlockBufferGetDataPointer(audioBlockBuffer, audioBlockBufferOffset, &lengthAtOffset, &totalLength, (char **)(&samples));
				
				
				int numSamplesToRead = 1;
				for (int i = 0; i < numSamplesToRead; i++) {
					
					for (int j = 0; j < numSamples / numSamplesToRead; j++) {
						SInt16 audioSample = samples[(i * (numSamples / numSamplesToRead)) + j];
						buffer[pos+j] = (float)(audioSample / SINT16_MAX);
						
					}
					
//					SInt16 subSet[numSamples / numSamplesToRead];
//					for (int j = 0; j < numSamples / numSamplesToRead; j++)
//						subSet[j] = samples[(i * (numSamples / numSamplesToRead)) + j];
//					
//					self.lastAudioSample = [self maxValueInArray: subSet ofSize: numSamples / numSamplesToRead];
//					
//					double scaledSample = (double) ((self.lastAudioSample / SINT16_MAX));
//					
//					[audioDisplayDelegate addX:scaledSample];
				}
				
				pos+=numSamples;
				
				
				
				
				
				NSLog(@"got audio, totalLength: %i",totalLength);
				
			}
		}
		
		if (!bVideoDone) {
			CMSampleBufferRef sampleBuffer = [videoOutput copyNextSampleBuffer];
			if (sampleBuffer == NULL) {
				bVideoDone = true;
			} else {
				NSLog(@"got video: %i",video->textures.size());
				//			CMTime timestamp = CMSampleBufferGetPresentationTimeStamp( sampleBuffer );
				//			if (CMTIME_IS_VALID( self.previousTimestamp ))
				//				self.videoFrameRate = 1.0 / CMTimeGetSeconds( CMTimeSubtract( timestamp, self.previousTimestamp ) );
				//			
				//			previousTimestamp = timestamp;
				//			
				
				
				if (video->textures.empty() )
				{
					CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);
					videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
					NSLog(@"videoDimensions: %i %i",videoDimensions.width,videoDimensions.height);
					
					
					
					CMVideoCodecType type = CMFormatDescriptionGetMediaSubType(formatDesc);
#if defined(__LITTLE_ENDIAN__)
					type = OSSwapInt32(type);
#endif
					videoType = type;
					
					videoTexture = createVideoTextue(videoDimensions.width,videoDimensions.height);
					
					float videoAspectRatio = (float)videoDimensions.width/(float)videoDimensions.height;
					float textureAspectRatio = (float)video->textureWidth/(float)video->textureHeight;
					
					if (textureAspectRatio>videoAspectRatio) {
						renderHeight = video->textureHeight;
						renderWidth = videoAspectRatio * renderHeight;
						video->heightFraction = 1;
						video->widthFraction = renderWidth/(float)video->textureWidth;
					} else {
						renderWidth = video->textureWidth;
						renderHeight =  renderWidth / videoAspectRatio;
						video->widthFraction = 1;
						video->heightFraction = renderHeight/(float)video->textureHeight;
					}
					
					NSLog(@"renderDimensions: %.0f %.0f",renderWidth,renderHeight);
					NSLog(@"textureFractions: %.2f %.2f",video->widthFraction,video->heightFraction);


					
					
				}
				
				CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
				CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
				
			
				glBindTexture(GL_TEXTURE_2D, videoTexture);
				
				unsigned char* linebase = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
				
				
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
				
				CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
				
				GLuint texture;
				
				glGenTextures(1, &texture);
				
				
				
				glBindTexture(GL_TEXTURE_2D, texture);
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, video->textureWidth, video->textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, NULL);     // check if this is right
				
				//glBindTexture(GL_TEXTURE_2D, 0);
				
				fbo.begin(texture);
			
				//renderCameraToSprite(videoTexture, video->textureWidth, video->textureHeight);
				//float w = width; // normalizing each camera to 480 * 360
				//float h = (float)videoDimensions.height / (float)videoDimensions.width * w ;
				renderCameraToSprite(videoTexture, renderWidth, renderHeight);
				//renderCameraToSprite(videoTexture, videoDimensions.width, videoDimensions.height);
				
				fbo.end();
				
				video->textures.push_back(texture);
			}

		}
		
	}
	
	fbo.pop();
	
	NSLog(@"status: %i",reader.status);
	
	if (reader.status == AVAssetReaderStatusFailed) {
		NSLog(@"error: %@",[reader.error description]);
	}
	
	int numBuffers = pos / video->bufferSize;
	numBuffers+= pos % video->bufferSize ? 1 : 0;
	video->audio.setup(video->bufferSize,numBuffers);  //video.sampleRate * video.sampleLength / (1000 * bufferSize)
	
	for (int i=0; i<pos; i++) {
		video->audio.buffer[i] = buffer[i];
	}
	
	delete buffer;
	
	// no need to zero pad the end because it is done in audio.setup()
	
	
	video->sampleLength = 1000*video->numFrames/video->fps;
	
	video->numFrames = video->textures.size();
	
}


