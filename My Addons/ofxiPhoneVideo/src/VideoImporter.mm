//
//  VideoImporter.m
//  VideoToTextures
//
//  Created by Roee Kremer on 12/22/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "VideoImporter.h"
#import <AVFoundation/AVFoundation.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "ofxiPhoneVideo.h"

@interface VideoImporter (Internal)
-(GLuint)createVideoTextuerUsingWidth:(GLuint)w Height:(GLuint)h;
@end

@implementation VideoImporter


@synthesize videoDimensions;
@synthesize videoType;


+ (id) videoImporter {
	return [[[VideoImporter alloc] init] autorelease];
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
	
//	float w = videoDimensions.width;
//	float h = videoDimensions.height;
//	
//	GLfloat spriteVertices[] =  {
//		w,h,0,   
//		w,0,0,   
//		0,h,0, 
//		0,0,0};
	
	GLfloat spriteVertices[] =  {
		0,0,0,   
		320,0,0,   
		0,480,0, 
		320,480,0};
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, 320, 0, 480, 0, 1);
	
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisableClientState(GL_COLOR_ARRAY);
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


- (void)import:(NSURL*)videoURL withVideo:(ofxiPhoneVideo *)video //withContext:(EAGLContext *)contextA withCompletionHandler:(void (^)(void))completionHandler 
{	
	AVURLAsset *asset;
	AVAssetReaderTrackOutput *output;
		
	asset = [AVURLAsset URLAssetWithURL:videoURL options:nil];
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
				self.videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
				NSLog(@"videoDimensions: %i %i",self.videoDimensions.width,self.videoDimensions.height);
				
				CMVideoCodecType type = CMFormatDescriptionGetMediaSubType(formatDesc);
#if defined(__LITTLE_ENDIAN__)
				type = OSSwapInt32(type);
#endif
				self.videoType = type;
				
				
			}
						
			CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
			CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
			
			
			
			uint m_textureHandle = [self createVideoTextuerUsingWidth:videoDimensions.width Height:videoDimensions.height];
			video->textures.push_back(m_textureHandle);
			glBindTexture(GL_TEXTURE_2D, m_textureHandle);
			
			unsigned char* linebase = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
			
			
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
			
			
			CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
			i++;
		}
		
	}
		
	
	
}
@end
