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

void ofxiVideoLoader::setup(ofxiPhoneVideo *video) {
	this->video = video;
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

void ofxiVideoLoader::load(string filename) {
	AVURLAsset *asset;
	AVAssetReaderTrackOutput *output;
	
	asset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:[NSString stringWithFormat:@"%s",filename.c_str()]] options:nil];
	NSArray *tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
	//NSDictionary* audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM ], AVFormatIDKey,nil];
	
	NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey, 
										   [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],(NSString*)kCVPixelBufferPixelFormatTypeKey,
										   /*[NSNumber numberWithUnsignedInt:size.width],(NSString*)kCVPixelBufferWidthKey,
											[NSNumber numberWithUnsignedInt:size.height],(NSString*)kCVPixelBufferHeightKey,*/nil];
	
	
	output = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[tracks objectAtIndex:0] outputSettings:pixelBufferAttributes];
	NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[output mediaType]);
	
	NSError *error = nil;
	AVAssetReader *reader = [AVAssetReader assetReaderWithAsset:asset error:&error];
	
	if (error) {
		NSLog(@"AVAssetReader: %@",[error description]);
	}
	
	BOOL test = [reader canAddOutput:output];
	NSLog(@"can add output: %i",test);
	[reader addOutput:output];
	
	//	EAGLContext* contextB = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:contextA.sharegroup];
	//	[EAGLContext setCurrentContext:contextB];
	
	[reader startReading];
	
	int i=0;
	video->textures.clear();
	
	CMVideoDimensions videoDimensions;
	CMVideoCodecType videoType;
	
	while (reader.status == AVAssetReaderStatusReading) {
		CMSampleBufferRef  sampleBuffer = [output copyNextSampleBuffer];
		if (sampleBuffer != NULL) {
			NSLog(@"got sampleBuffer %i",i);
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
				
				
			}
			
			CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
			CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
			
			
			GLuint texture = createVideoTextue(videoDimensions.width,videoDimensions.height);
			
			video->textures.push_back(texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			
			unsigned char* linebase = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
			
			
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
			
			
			CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
			i++;
		}
		
	}
	
	video->numFrames = video->textures.size();
	
}


