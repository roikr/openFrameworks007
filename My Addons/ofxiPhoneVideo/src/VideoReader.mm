//
//  VideoReader.m
//  VideoScrambler
//
//  Created by Roee Kremer on 1/23/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "VideoReader.h"

#import <AVFoundation/AVFoundation.h>

#define SINT16_MAX 32767.0

@implementation VideoReader

@synthesize reader,videoOutput,audioOutput,videoTexture,frameRate,width,height,readPos,frame;

+(id)videoReaderWithURL:(NSURL*)videoURL 
{
	return [[[VideoReader alloc] initWithURL:videoURL] autorelease];
}

-(id)initWithURL:(NSURL*)videoURL {
	
	if (self=[super init]) {
		
		NSError *error;
		BOOL test;
	
		AVURLAsset * asset = [AVURLAsset URLAssetWithURL:videoURL options:nil];
		self.reader = [AVAssetReader assetReaderWithAsset:asset error:&error];
		if (error) {
			NSLog(@"AVAssetReader: %@",[error description]);
		}
		
		
		
		NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey, 
											   [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],(NSString*)kCVPixelBufferPixelFormatTypeKey,
											   /*[NSNumber numberWithUnsignedInt:size.width],(NSString*)kCVPixelBufferWidthKey,
												[NSNumber numberWithUnsignedInt:size.height],(NSString*)kCVPixelBufferHeightKey,*/nil];
		
		
		self.videoOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0] outputSettings:pixelBufferAttributes];
		NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[self.videoOutput mediaType]);
		test = [reader canAddOutput:videoOutput];
		NSLog(@"can add video output: %i",test);
		[reader addOutput:videoOutput];
				
		
		NSDictionary* audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM ], AVFormatIDKey,nil];
		
		self.audioOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[[asset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0] outputSettings:audioOutputSettings];
		NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[self.audioOutput mediaType]);
		test = [reader canAddOutput:audioOutput];
		NSLog(@"can add audio output: %i",test);
		[reader addOutput:audioOutput];
		
		
		frame = 0;
		frameRate = 0;
		bReadAudio = true;
		bReadVideo = true;
		
		//buffer = (float *) malloc(BUFFER_SIZE * sizeof(float));
		memset(_buffer, 0, RING_BUFFER_SIZE*sizeof(float));
		readPos = 0;
		
		[reader startReading];
	}
	
	
	return self;
}



-(GLuint)createVideoTextuerUsingWidth:(GLuint)w Height:(GLuint)h
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

-(void)renderCameraToSprite:(uint)text
{
	if (!text)
		return;
	
	GLfloat spriteTexcoords[] = {
		1.0f,1.0f,   
		1.0f,0.0f,
		0,1.0f,   
		0.0f,0,};
	
	float w = 0.5; 
	float h= w /(float)height  *  (float)width   ;
	
	
	GLfloat spriteVertices[] = {
        -w, -h, 0,
        w, -h, 0,
        -w,  h, 0,
        w,  h, 0
    };
	
	
	//	GLfloat spriteVertices[] =  {
	//		0,0,0,   
	//		320,0,0,   
	//		0,480,0, 
	//		320,480,0};
	
	//	glMatrixMode(GL_PROJECTION);
	//	glLoadIdentity();
	//	glOrthof(0, 320, 0, 480, 0, 1);
	//	
	//    glMatrixMode(GL_MODELVIEW);
	//    glLoadIdentity();
	
	//	glDisable(GL_DEPTH_TEST);
	//	glDisableClientState(GL_COLOR_ARRAY);
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

// Returns array member with greatest absolute value
- (SInt16) maxValueInArray: (SInt16[]) values ofSize: (unsigned int) size {
	
	SInt16 max = 0;
	
	for (int i = 0; i < size; i++)
		if (abs(values[i]) > max)
			max = values[i];
	
	return max;
}


-(bool)readAudio {
	
	CMItemCount numSamples = 0;
	
	if (reader.status == AVAssetReaderStatusReading && bReadAudio) {
		CMSampleBufferRef  sampleBuffer = [audioOutput copyNextSampleBuffer];
		if (sampleBuffer != NULL) {
			numSamples = CMSampleBufferGetNumSamples(sampleBuffer);
			NSUInteger channelIndex = 0;
			
			if (!readPos) {
				CMFormatDescriptionRef formarDesc = CMSampleBufferGetFormatDescription (sampleBuffer);
				const AudioStreamBasicDescription * streamDesc = CMAudioFormatDescriptionGetStreamBasicDescription (formarDesc);
				//printf("here");
			}
						
			CMBlockBufferRef audioBlockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
			size_t audioBlockBufferOffset = (channelIndex * numSamples * sizeof(SInt16));
			size_t lengthAtOffset = 0;
			size_t totalLength = 0;
			SInt16 *samples = NULL;
			CMBlockBufferGetDataPointer(audioBlockBuffer, audioBlockBufferOffset, &lengthAtOffset, &totalLength, (char **)(&samples));
			//NSLog(@"numSamples: %i, audioBlockBufferOffset: %i, lengthAtOffset: %i, totalLength %i",numSamples,audioBlockBufferOffset,lengthAtOffset,totalLength);
			int numSamplesToRead = 1;
			for (int i = 0; i < numSamplesToRead; i++) {
				
				//SInt16 subSet[numSamples / numSamplesToRead];
//				for (int j = 0; j < numSamples / numSamplesToRead; j++)
//					subSet[j] = samples[(i * (numSamples / numSamplesToRead)) + j];
//				
//				lastAudioSample = [self maxValueInArray: subSet ofSize: numSamples / numSamplesToRead];
				
				
				
				for (int j = 0; j < numSamples / numSamplesToRead; j++) {
					
					SInt16 sample = samples[(i * (numSamples / numSamplesToRead)) + j];
					_buffer[(readPos+j) % RING_BUFFER_SIZE]= (float)sample / SINT16_MAX;
				}
					
				
				//lastAudioSample = [self maxValueInArray: subSet ofSize: numSamples / numSamplesToRead];
				//double scaledSample = (double) ((lastAudioSample / SINT16_MAX));
				
			}
			
			readPos+=numSamples;
						
			
			
			CFRelease(sampleBuffer);
		} else {
			bReadAudio = reader.status != AVAssetReaderStatusReading;
		}
	}
	
	return numSamples!=0;
}


-(float *)buffer {
	return _buffer;
}


-(void)updateVideo
{
	
	
	if (reader.status == AVAssetReaderStatusReading && bReadVideo) {
		CMSampleBufferRef  sampleBuffer = [videoOutput copyNextSampleBuffer];
		if (sampleBuffer != NULL) {
			
			CMTime timestamp = CMSampleBufferGetPresentationTimeStamp( sampleBuffer );
			if (CMTIME_IS_VALID( previousTimestamp ))
				frameRate = 1.0 / CMTimeGetSeconds( CMTimeSubtract( timestamp, previousTimestamp ) );
			
			previousTimestamp = timestamp;
			
			if (!videoTexture )
			{
				CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);
				CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
				width = videoDimensions.width;
				height = videoDimensions.height;
				NSLog(@"videoDimensions: %i %i",width,height);
				
				CMVideoCodecType type = CMFormatDescriptionGetMediaSubType(formatDesc);
#if defined(__LITTLE_ENDIAN__)
				type = OSSwapInt32(type);
#endif
				videoType = type;
				videoTexture = [self createVideoTextuerUsingWidth:width Height:height];
			}
		
			CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
			CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
			
			glBindTexture(GL_TEXTURE_2D, videoTexture);
			
			unsigned char* linebase = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
						
			CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
			CFRelease(sampleBuffer);
			
			//NSLog(@"sample: %i, frameRate: %2.1f",framesCounter,frameRate);
			frame++;
			
		}  else {
			bReadVideo = false;
			if (videoTexture) {
				glDeleteTextures(1, &videoTexture);
				videoTexture = 0;
				NSLog(@"video end");
			}
		}	
	}
}

- (void)dealloc {
    [super dealloc];
}


@end
