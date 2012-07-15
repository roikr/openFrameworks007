//
//  StillCamera.h
//  stillExample
//
//  Created by Roee Kremer on 7/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h> // for CMVideoCodecType

enum {
    STILL_CAMERA_STATE_INITIALIZED,
    STILL_CAMERA_STATE_PLAYING,
    STILL_CAMERA_STATE_STOPPED
};


@class EAGLContext;
@class EAGLSharegroup;

@interface StillCamera : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate> {
    NSMutableArray *previousSecondTimestamps;
	Float64 videoFrameRate;
	CMVideoDimensions videoDimensions;
	CMVideoCodecType videoType;
    
	AVCaptureSession *captureSession;
	AVCaptureConnection *videoConnection;
	
	AVCaptureVideoOrientation referenceOrientation;
	AVCaptureVideoOrientation videoOrientation;
    CMBufferQueueRef previewBufferQueue;
    
    AVCaptureStillImageOutput *stillImageOutput;
    
    BOOL capturingStillImage;

    int state;
    
    CMBufferQueueRef imagesBufferQueue;
    
}


@property (readonly) Float64 videoFrameRate;
@property (readonly) CMVideoDimensions videoDimensions;
@property (readonly) CMVideoCodecType videoType;
@property (readwrite) AVCaptureVideoOrientation referenceOrientation;
@property (readonly) AVCaptureVideoOrientation videoOrientation;
@property (readonly) int state;
@property (readonly, getter=isCapturingStillImage) BOOL capturingStillImage;
@property (readonly) CMBufferQueueRef previewBufferQueue;
@property (readonly) CMBufferQueueRef imagesBufferQueue;


- (id)init;
- (void)takePicture;
- (void)stop;
- (AVCaptureVideoOrientation)avOrientationForDeviceOrientation:(UIDeviceOrientation)deviceOrientation;

@end

