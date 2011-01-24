//
//  VideoReader.h
//  VideoScrambler
//
//  Created by Roee Kremer on 1/23/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#define RING_BUFFER_SIZE 65536

@class AVAssetReader;
@class AVAssetReaderTrackOutput;

@interface VideoReader : NSObject {
	GLuint videoTexture;
	
	CMVideoCodecType videoType;
	AVAssetReader *reader;
	AVAssetReaderTrackOutput *videoOutput;
	AVAssetReaderTrackOutput *audioOutput;
	int frame;
	
	
	float frameRate;
	CMTime previousTimestamp;
	
	int width;
	int height;
	
	bool bReadAudio;
	bool bReadVideo;

	float _buffer[RING_BUFFER_SIZE];
	int readPos;
}

@property (readonly) GLuint videoTexture;
@property (readonly) float frameRate;
@property (readonly) int width;
@property (readonly) int height;
@property (nonatomic,retain) AVAssetReader *reader;
@property (nonatomic,retain) AVAssetReaderTrackOutput *videoOutput;
@property (nonatomic,retain) AVAssetReaderTrackOutput *audioOutput;
@property (readonly) int readPos;
@property (readonly) float* buffer;
@property (readonly) int frame;



+(id)videoReaderWithURL:(NSURL*)videoURL;
-(id)initWithURL:(NSURL*)videoURL;
-(GLuint)createVideoTextuerUsingWidth:(GLuint)w Height:(GLuint)h;
- (void) renderCameraToSprite:(uint)text;
- (void) updateVideo;
-(bool)readAudio;

@end
