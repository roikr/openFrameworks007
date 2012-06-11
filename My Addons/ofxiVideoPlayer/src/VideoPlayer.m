//
//  VideoPlayer.m
//  ofxiVideoPlayerExample
//
//  Created by Roee Kremer on 5/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "VideoPlayer.h"
#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CoreVideo.h>

#define SINT16_MAX 32767.0

@implementation VideoPlayer

@synthesize frameRate,width,height,state,readPos,frame,numChannels,bReadAudio;

-(id)initWithURL:(NSURL*)videoURL context:(EAGLContext *)context{
	
	if (self=[super init]) {
		
        state = VIDEO_PLAYER_STATE_INITIALIZED;
        
		NSError *error;
		BOOL test;
        
        _videoTexture = NULL; 
        bSetType = false;
        
        //-- Create CVOpenGLESTextureCacheRef for optimal CVImageBufferRef to GLES texture conversion.
        CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, (__bridge void *)context, NULL, &_videoTextureCache);
        if (err) 
        {
            NSLog(@"Error at CVOpenGLESTextureCacheCreate %d", err);
        }
        
		//NSDictionary *options = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
		
		AVURLAsset * asset = [AVURLAsset URLAssetWithURL:videoURL options:nil];
		reader = [[AVAssetReader alloc] initWithAsset:asset error:&error] ;
		if (error) {
			NSLog(@"AVAssetReader: %@",[error description]);
		}
		
		
		
		NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey, 
											   [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],(NSString*)kCVPixelBufferPixelFormatTypeKey,
											   /*[NSNumber numberWithUnsignedInt:size.width],(NSString*)kCVPixelBufferWidthKey,
												[NSNumber numberWithUnsignedInt:size.height],(NSString*)kCVPixelBufferHeightKey,*/nil];
		
		
		videoOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:[[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0] outputSettings:pixelBufferAttributes];
		NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[videoOutput mediaType]);
		test = [reader canAddOutput:videoOutput];
		NSLog(@"can add video output: %i",test);
		[reader addOutput:videoOutput];
        
		
		NSDictionary* audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM ], AVFormatIDKey,nil];
		
        bReadAudio = [[asset tracksWithMediaType:AVMediaTypeAudio] count];
        
        if (bReadAudio) {
            audioOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:[[asset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0] outputSettings:audioOutputSettings];
            NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[audioOutput mediaType]);
            test = [reader canAddOutput:audioOutput];
            NSLog(@"can add audio output: %i",test);
            [reader addOutput:audioOutput];
        }
        
		
		
		
		frame = 0;
		frameRate = 0;
		bReadVideo = true;
		
		//buffer = (float *) malloc(BUFFER_SIZE * sizeof(float));
		memset(_buffer, 0, RING_BUFFER_SIZE*sizeof(float));
		readPos = 0;
		
		[reader startReading];
	}
	
	
	return self;
}

-(void) stop {
    [reader cancelReading];
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
				numChannels = streamDesc->mChannelsPerFrame;
				
			}
            
			CMBlockBufferRef audioBlockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
			size_t audioBlockBufferOffset = (channelIndex * numSamples * sizeof(SInt16));
			size_t lengthAtOffset = 0;
			size_t totalLength = 0;
			SInt16 *samples = NULL;
			CMBlockBufferGetDataPointer(audioBlockBuffer, audioBlockBufferOffset, &lengthAtOffset, &totalLength, (char **)(&samples));
			//NSLog(@"numSamples: %i, audioBlockBufferOffset: %i, lengthAtOffset: %i, totalLength %i",numSamples,audioBlockBufferOffset,lengthAtOffset,totalLength);
			
			for (int i=0; i<numSamples*numChannels; i++) {
				SInt16 sample = samples[i];
				_buffer[(readPos+i) % RING_BUFFER_SIZE]= (float)sample / SINT16_MAX;
			}
            
			readPos+=numSamples*numChannels;
            
			
			
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
	if (!_videoTextureCache)
    {
        NSLog(@"No video texture cache");
        state = VIDEO_PLAYER_STATE_FINISHED;
        return;
    }
	
	if (reader.status == AVAssetReaderStatusReading && bReadVideo) {
		CMSampleBufferRef  sampleBuffer = [videoOutput copyNextSampleBuffer];
		if (sampleBuffer != NULL) {
			
			CMTime timestamp = CMSampleBufferGetPresentationTimeStamp( sampleBuffer );
			if (CMTIME_IS_VALID( previousTimestamp ))
				frameRate = 1.0 / CMTimeGetSeconds( CMTimeSubtract( timestamp, previousTimestamp ) );
			
			previousTimestamp = timestamp;
			
			if (state == VIDEO_PLAYER_STATE_INITIALIZED )
			{
                state = VIDEO_PLAYER_STATE_PLAYING;
				bSetType = true;
                CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);
                //				CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
                //				width = videoDimensions.width;
                //				height = videoDimensions.height;
                //				NSLog(@"videoDimensions: %i %i",width,height);
				
				CMVideoCodecType type = CMFormatDescriptionGetMediaSubType(formatDesc);
#if defined(__LITTLE_ENDIAN__)
				type = OSSwapInt32(type);
#endif
				videoType = type;
			}
            
            CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
            width = CVPixelBufferGetWidth(pixelBuffer);
            height = CVPixelBufferGetHeight(pixelBuffer);
            
            [self cleanUpTextures];
            
            CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, 
                                                                        _videoTextureCache,
                                                                        pixelBuffer,
                                                                        NULL,
                                                                        GL_TEXTURE_2D,
                                                                        GL_RGBA,
                                                                        width,
                                                                        height,
                                                                        GL_BGRA,
                                                                        GL_UNSIGNED_BYTE,
                                                                        0,
                                                                        &_videoTexture);
            
            
            if (!_videoTexture || err) {
                NSLog(@"CVOpenGLESTextureCacheCreateTextureFromImage failed (error: %d)", err);  
            }
            
            CFRelease(sampleBuffer);
			frame++;
			
		}  else {
			bReadVideo = false;
			
            [self cleanUpTextures];
            state = VIDEO_PLAYER_STATE_FINISHED; // roikr: TODO check sound also
            NSLog(@"video end");
			
		}	
	} 

    if (reader.status == AVAssetReaderStatusCancelled) {
        [self cleanUpTextures];
        state = VIDEO_PLAYER_STATE_FINISHED; 
    }
}

- (void) bind {
    glBindTexture(CVOpenGLESTextureGetTarget(_videoTexture), CVOpenGLESTextureGetName(_videoTexture));
    
    // Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
}

- (void) unbind {
    glBindTexture(CVOpenGLESTextureGetTarget(_videoTexture), 0);
}

- (void)cleanUpTextures
{    
    if (_videoTexture)
    {
        CFRelease(_videoTexture);
        _videoTexture = NULL;        
    }
    
    
    
    // Periodic texture cache flush every frame
    CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);
}


- (void)dealloc {
    [self cleanUpTextures];
    
    if (_videoTextureCache) {
        CFRelease(_videoTextureCache);
        _videoTextureCache = 0;
    }
    
    [reader release];
	if ( audioOutput != nil )
		[audioOutput release];
	if ( videoOutput != nil )
		[videoOutput release];
    
    [super dealloc];
}


@end
