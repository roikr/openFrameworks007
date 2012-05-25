//
//  VideoPlayer.h
//  ofxiVideoPlayerExample
//
//  Created by Roee Kremer on 5/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>


#define RING_BUFFER_SIZE 65536

enum {
    VIDEO_PLAYER_STATE_INITIALIZED,
    VIDEO_PLAYER_STATE_PLAYING,
    VIDEO_PLAYER_STATE_STOPPING,
    VIDEO_PLAYER_STATE_FINISHED
};

@class AVAssetReader;
@class AVAssetReaderTrackOutput;
@class EAGLContext;

@interface VideoPlayer : NSObject {
	
	
	CMVideoCodecType videoType;
	AVAssetReader *reader;
	AVAssetReaderTrackOutput *videoOutput;
	AVAssetReaderTrackOutput *audioOutput;
	int frame;
	
	
	float frameRate;
	CMTime previousTimestamp;
	
    bool bStarted;
    bool bSetType;
    int width;
	int height;
	
	bool bReadAudio;
	bool bReadVideo;
    
	float _buffer[RING_BUFFER_SIZE];
	int readPos;
	
	int numChannels;
    
    CVOpenGLESTextureCacheRef _videoTextureCache;
    CVOpenGLESTextureRef _videoTexture;
    
    int state;
}

@property (readonly) float frameRate;
@property (readonly) int width;
@property (readonly) int height;
@property (readonly) int readPos;
@property (readonly) float* buffer;
@property (readonly) int frame;
@property (readonly) int numChannels;

@property (readonly) int state;


- (id)initWithURL:(NSURL*)videoURL context:(EAGLContext *)context;
- (void) updateVideo;
- (bool) readAudio;
- (void) bind;
- (void) unbind;
- (void) stop;

@end


