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

void ofxiVideoLoader::setup() {
	
}

void ofxiVideoLoader::addVideo(ofxiPhoneVideo *video) {
	videos.push_back(video);
}


void ofxiVideoLoader::loadAudio(ofxiPhoneVideo *video,string filename) {
	
	
	NSString *path = [NSString stringWithFormat:@"%s",filename.c_str()];
	
	AVURLAsset *asset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:path] options:nil];
	
	NSError *error = nil;
	
	AVAssetReader *reader = [AVAssetReader assetReaderWithAsset:asset error:&error];
	
	if (error) {
		NSLog(@"AVAssetReader: %@",[error description]);
	}
	
		
	
	NSArray *audioTracks = [asset tracksWithMediaType:AVMediaTypeAudio];
	NSDictionary* audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM ], AVFormatIDKey,
										 //																				[NSNumber numberWithFloat:video->sampleRate],AVSampleRateKey,
										 nil];
	
	AVAssetReaderTrackOutput * audioOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[audioTracks objectAtIndex:0] outputSettings:audioOutputSettings];
	NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[audioOutput mediaType]);
	
	
	BOOL test = [reader canAddOutput:audioOutput];
	NSLog(@"can add output: %i",test);
	[reader addOutput:audioOutput];
	
	[reader startReading];
	
	float *buffer = new float[(int)(video->sampleRate * 10)]; // max 10 sec
	
	
	int pos = 0;
		
	while (reader.status == AVAssetReaderStatusReading) {
		
		
		CMSampleBufferRef  sampleBuffer = [audioOutput copyNextSampleBuffer];
		if (sampleBuffer != NULL) {
		
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
				
			}
			
			pos+=numSamples;
			
			NSLog(@"samplesRead: %i, pos: %i",numSamples,pos);
			CFRelease(sampleBuffer);
			
		}
	}
	
	
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
	
	// no need to zero pad the end because it is done in audio.setup()
	delete buffer;
	
}



GLuint ofxiVideoLoader::createVideoTextue(GLuint w,GLuint h)
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

void ofxiVideoLoader::loadVideo(string filename) {
	
	
	NSString *path = [NSString stringWithFormat:@"%s",filename.c_str()];
	
	AVURLAsset *asset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:path] options:nil];
	
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
	
		
	BOOL test = [reader canAddOutput:videoOutput];
	NSLog(@"can add output: %i",test);
	[reader addOutput:videoOutput];

	ofxiFBO fbo;
	fbo.setup();
	GLuint videoTexture;
	
		
	//	EAGLContext* contextB = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:contextA.sharegroup];
	//	[EAGLContext setCurrentContext:contextB];
	
	vector<ofxiPhoneVideo*>::iterator iter = videos.begin();

	(*iter)->textures.clear();
	
	
	CMVideoDimensions videoDimensions;
	CMVideoCodecType videoType;
	
	
	
	fbo.push((*iter)->textureWidth, (*iter)->textureHeight);
	
	float renderWidth;
	float renderHeight;
	
	//reader.timeRange.start.value = 0;
	
	[reader startReading];
	
	while (reader.status == AVAssetReaderStatusReading) {
		
		
		CMSampleBufferRef sampleBuffer = [videoOutput copyNextSampleBuffer];
		if (sampleBuffer != NULL) {
		
			printf(".");
			//NSLog(@"got video: %i",(*iter)->textures.size());
			//			CMTime timestamp = CMSampleBufferGetPresentationTimeStamp( sampleBuffer );
			//			if (CMTIME_IS_VALID( self.previousTimestamp ))
			//				self.videoFrameRate = 1.0 / CMTimeGetSeconds( CMTimeSubtract( timestamp, self.previousTimestamp ) );
			//			
			//			previousTimestamp = timestamp;
			//			
			
			
			if (iter==videos.begin() && (*iter)->textures.empty() )
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
				float textureAspectRatio = (float)(*iter)->textureWidth/(float)(*iter)->textureHeight;
				
				if (textureAspectRatio>videoAspectRatio) {
					renderHeight = (*iter)->textureHeight;
					renderWidth = videoAspectRatio * renderHeight;
					(*iter)->heightFraction = 1;
					(*iter)->widthFraction = renderWidth/(float)(*iter)->textureWidth;
				} else {
					renderWidth = (*iter)->textureWidth;
					renderHeight =  renderWidth / videoAspectRatio;
					(*iter)->widthFraction = 1;
					(*iter)->heightFraction = renderHeight/(float)(*iter)->textureHeight;
				}
				
				NSLog(@"renderDimensions: %.0f %.0f",renderWidth,renderHeight);
				NSLog(@"textureFractions: %.2f %.2f",(*iter)->widthFraction,(*iter)->heightFraction);


				
				
			}
			
			CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
			CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
			glBindTexture(GL_TEXTURE_2D, videoTexture);
			unsigned char* linebase = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
			CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
			CFRelease(sampleBuffer);
			
	
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (*iter)->textureWidth, (*iter)->textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, NULL);     // check if this is right
			
			//glBindTexture(GL_TEXTURE_2D, 0);
			
			fbo.begin(texture);
		
			//renderCameraToSprite(videoTexture, video->textureWidth, video->textureHeight);
			//float w = width; // normalizing each camera to 480 * 360
			//float h = (float)videoDimensions.height / (float)videoDimensions.width * w ;
			renderCameraToSprite(videoTexture, renderWidth, renderHeight);
			//renderCameraToSprite(videoTexture, videoDimensions.width, videoDimensions.height);
			
			fbo.end();
			
			if (iter!=videos.end()) {
				(*iter)->textures.push_back(texture);
				
				if ((*iter)->textures.size() >= (*iter)->numFrames   ) {
					
					iter++;
				}
			}
			
			
			
			
		}

	}
		
	printf("\n");
	fbo.pop();
	
		
	NSLog(@"status: %i",reader.status);
	
	if (reader.status == AVAssetReaderStatusFailed) {
		NSLog(@"error: %@",[reader.error description]);
	}
	
	
	
	
}


