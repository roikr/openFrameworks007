//
//  StillCamera.m
//  stillExample
//
//  Created by Roee Kremer on 7/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "StillCamera.h"
#import <CoreVideo/CoreVideo.h>
//#import <MobileCoreServices/MobileCoreServices.h>
//#import <AssetsLibrary/AssetsLibrary.h>

// used for KVO observation of the @"capturingStillImage" property to perform flash bulb animation
static const NSString *AVCaptureStillImageIsCapturingStillImageContext =@"AVCaptureStillImageIsCapturingStillImageContext";


@interface StillCamera ()

// Redeclared as readwrite so that we can write to the property and still be atomic with external readers.
@property (readwrite) Float64 videoFrameRate;
@property (readwrite) CMVideoDimensions videoDimensions;
@property (readwrite) CMVideoCodecType videoType;
@property (readwrite) AVCaptureVideoOrientation videoOrientation;
@property(readwrite, getter=isCapturingStillImage) BOOL capturingStillImage;


@end

@implementation StillCamera

@synthesize videoFrameRate, videoDimensions, videoType;
@synthesize referenceOrientation;
@synthesize videoOrientation;
@synthesize capturingStillImage;
@synthesize state;
@synthesize previewBufferQueue;
@synthesize imagesBufferQueue;



#pragma mark Utilities

- (void) calculateFramerateAtTimestamp:(CMTime) timestamp
{
	[previousSecondTimestamps addObject:[NSValue valueWithCMTime:timestamp]];
    
	CMTime oneSecond = CMTimeMake( 1, 1 );
	CMTime oneSecondAgo = CMTimeSubtract( timestamp, oneSecond );
    
	while( CMTIME_COMPARE_INLINE( [[previousSecondTimestamps objectAtIndex:0] CMTimeValue], <, oneSecondAgo ) )
		[previousSecondTimestamps removeObjectAtIndex:0];
    
	Float64 newRate = (Float64) [previousSecondTimestamps count];
	self.videoFrameRate = (self.videoFrameRate + newRate) / 2;
}



#pragma mark Capture

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection 
{	
	CMFormatDescriptionRef formatDescription = CMSampleBufferGetFormatDescription(sampleBuffer);
    
	if ( connection == videoConnection ) {
		
		// Get framerate
		CMTime timestamp = CMSampleBufferGetPresentationTimeStamp( sampleBuffer );
		[self calculateFramerateAtTimestamp:timestamp];
        
		// Get frame dimensions (for onscreen display)
		if (self.videoDimensions.width == 0 && self.videoDimensions.height == 0) {
			self.videoDimensions = CMVideoFormatDescriptionGetDimensions( formatDescription );
            NSLog(@"videoDimensions: %i\t%i",videoDimensions.width,videoDimensions.height);
        }
		
		// Get buffer type
		if ( self.videoType == 0 )
			self.videoType = CMFormatDescriptionGetMediaSubType( formatDescription );
        
        if (state == STILL_CAMERA_STATE_INITIALIZED )
        {
            state = STILL_CAMERA_STATE_PLAYING;
        }
        
        // Enqueue it for preview.  This is a shallow queue, so if image processing is taking too long,
		// we'll drop this frame for preview (this keeps preview latency low).
		OSStatus err = CMBufferQueueEnqueue(previewBufferQueue, sampleBuffer);
		if ( !err ) {    
        }
    }
	
}



- (AVCaptureDevice *)videoDeviceWithPosition:(AVCaptureDevicePosition)position 
{
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in devices)
        if ([device position] == position)
            return device;
    
    return nil;
}

- (BOOL) setupCaptureSession 
{
	/*
     Overview: RosyWriter uses separate GCD queues for audio and video capture.  If a single GCD queue
     is used to deliver both audio and video buffers, and our video processing consistently takes
     too long, the delivery queue can back up, resulting in audio being dropped.
     
     When recording, RosyWriter creates a third GCD queue for calls to AVAssetWriter.  This ensures
     that AVAssetWriter is not called to start or finish writing from multiple threads simultaneously.
     
     RosyWriter uses AVCaptureSession's default preset, AVCaptureSessionPresetHigh.
	 */
    
    /*
	 * Create capture session
	 */
    captureSession = [[AVCaptureSession alloc] init];
//    captureSession.sessionPreset = AVCaptureSessionPresetPhoto; // roikr: uncomment for photo resolution
 	/*
	 * Create video connection
	 */
    AVCaptureDeviceInput *videoIn = [[AVCaptureDeviceInput alloc] initWithDevice:[self videoDeviceWithPosition:AVCaptureDevicePositionBack] error:nil];
    if ([captureSession canAddInput:videoIn])
        [captureSession addInput:videoIn];
	[videoIn release];
    
	AVCaptureVideoDataOutput *videoOut = [[AVCaptureVideoDataOutput alloc] init];
	/*
     RosyWriter prefers to discard late video frames early in the capture pipeline, since its
     processing can take longer than real-time on some platforms (such as iPhone 3GS).
     Clients whose image processing is faster than real-time should consider setting AVCaptureVideoDataOutput's
     alwaysDiscardsLateVideoFrames property to NO. 
	 */
	[videoOut setAlwaysDiscardsLateVideoFrames:YES];
//	[videoOut setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    
    NSDictionary *pixelBufferAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES], kCVPixelBufferOpenGLCompatibilityKey, 
                                           [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA],(NSString*)kCVPixelBufferPixelFormatTypeKey,
                                           /*[NSNumber numberWithUnsignedInt:size.width],(NSString*)kCVPixelBufferWidthKey,
                                            [NSNumber numberWithUnsignedInt:size.height],(NSString*)kCVPixelBufferHeightKey,*/nil];
    [videoOut setVideoSettings:pixelBufferAttributes];
    
	dispatch_queue_t videoCaptureQueue = dispatch_queue_create("Video Capture Queue", DISPATCH_QUEUE_SERIAL);
	[videoOut setSampleBufferDelegate:self queue:videoCaptureQueue];
	dispatch_release(videoCaptureQueue);
	if ([captureSession canAddOutput:videoOut])
		[captureSession addOutput:videoOut];
	videoConnection = [videoOut connectionWithMediaType:AVMediaTypeVideo];
	self.videoOrientation = [videoConnection videoOrientation];
	[videoOut release];
    
    
    // Make a still image output
	stillImageOutput = [AVCaptureStillImageOutput new];
	[stillImageOutput addObserver:self forKeyPath:@"capturingStillImage" options:NSKeyValueObservingOptionNew context:AVCaptureStillImageIsCapturingStillImageContext];
	if ( [captureSession canAddOutput:stillImageOutput] )
		[captureSession addOutput:stillImageOutput];
    
	return YES;
}



-(id)init {
	
	if (self=[super init]) {
        
                
        previousSecondTimestamps = [[NSMutableArray alloc] init];
        referenceOrientation = UIDeviceOrientationPortrait;
        capturingStillImage = NO;
        
        state = STILL_CAMERA_STATE_INITIALIZED;
        
        // Create a shallow queue for buffers going to the display for preview.
        OSStatus err = CMBufferQueueCreate(kCFAllocatorDefault, 1, CMBufferQueueGetCallbacksForUnsortedSampleBuffers(), &previewBufferQueue);
        if (err) {
            NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
            NSLog(@"StillCamera init error: %@, reason: %@",[error localizedDescription],[error localizedFailureReason]);
        }
        
        // Create a shallow queue for still images buffers 
        err = CMBufferQueueCreate(kCFAllocatorDefault, 1, CMBufferQueueGetCallbacksForUnsortedSampleBuffers(), &imagesBufferQueue);
        if (err) {
            NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
            NSLog(@"StillCamera init error: %@, reason: %@",[error localizedDescription],[error localizedFailureReason]);
        }
        
        
        if ( !captureSession )
            [self setupCaptureSession];
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(captureSessionStoppedRunningNotification:) name:AVCaptureSessionDidStopRunningNotification object:captureSession];
        
        if ( !captureSession.isRunning )
            [captureSession startRunning];        
		
    }
	
	return self;
}

// utility routing used during image capture to set up capture orientation
- (AVCaptureVideoOrientation)avOrientationForDeviceOrientation:(UIDeviceOrientation)deviceOrientation
{
	AVCaptureVideoOrientation result = deviceOrientation;
	if ( deviceOrientation == UIDeviceOrientationLandscapeLeft )
		result = AVCaptureVideoOrientationLandscapeRight;
	else if ( deviceOrientation == UIDeviceOrientationLandscapeRight )
		result = AVCaptureVideoOrientationLandscapeLeft;
	return result;
}


// main action method to take a still image -- if face detection has been turned on and a face has been detected
// the square overlay will be composited on top of the captured image and saved to the camera roll
- (void)takePicture
{
	self.capturingStillImage = YES;
    // Find out the current orientation and tell the still image output.
	AVCaptureConnection *stillImageConnection = [stillImageOutput connectionWithMediaType:AVMediaTypeVideo];
	UIDeviceOrientation curDeviceOrientation = [[UIDevice currentDevice] orientation];
	AVCaptureVideoOrientation avcaptureOrientation = [self avOrientationForDeviceOrientation:curDeviceOrientation];
	[stillImageConnection setVideoOrientation:avcaptureOrientation];
    //	[stillImageConnection setVideoScaleAndCropFactor:effectiveScale];
	
//    [stillImageOutput setOutputSettings:[NSDictionary dictionaryWithObject:AVVideoCodecJPEG forKey:AVVideoCodecKey]]; 
    
    [stillImageOutput setOutputSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA] 
                                                                    forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey]]; 
	
    
	[stillImageOutput captureStillImageAsynchronouslyFromConnection:stillImageConnection
          completionHandler:^(CMSampleBufferRef imageDataSampleBuffer, NSError *error) {
              
              dispatch_async(dispatch_get_main_queue(), ^{
                  //                  [self.delegate snapDone];
              });
              
              self.capturingStillImage = NO;
              
              if (error) {
                  NSLog(@"Take picture failed, error: %@, reason: %@",[error localizedDescription],[error localizedFailureReason]);
              }
              else {
                  {
//                      // Enqueue it for preview.  This is a shallow queue, so if image processing is taking too long,
//                      // we'll drop this frame for preview (this keeps preview latency low).
                      OSStatus err = CMBufferQueueEnqueue(imagesBufferQueue, imageDataSampleBuffer);
                      if ( !err ) {    
                      }
                      // trivial simple JPEG case
                     
                     
                      
//                      CFDictionaryRef attachments = CMCopyDictionaryOfAttachments(kCFAllocatorDefault, 
//                                                                                  imageDataSampleBuffer, 
//                                                                                  kCMAttachmentMode_ShouldPropagate);
//                      ALAssetsLibrary *library = [[ALAssetsLibrary alloc] init];
//                      [library writeImageDataToSavedPhotosAlbum:jpegData metadata:(id)attachments completionBlock:^(NSURL *assetURL, NSError *error) {
//                          if (error) {
//                              NSLog(@"Save to camera roll failed, error: %@, reason: %@",[error localizedDescription],[error localizedFailureReason]);
//                          }
//                      }];
//                      
//                      if (attachments)
//                          CFRelease(attachments);
//                      [library release];
                  }
              }
          }
	 ];
}


- (void) stop
{
    state = STILL_CAMERA_STATE_STOPPED;
    [captureSession stopRunning];
	if (captureSession)
		[[NSNotificationCenter defaultCenter] removeObserver:self name:AVCaptureSessionDidStopRunningNotification object:captureSession];
    
    
	[captureSession release];
	captureSession = nil;
    
    [stillImageOutput removeObserver:self forKeyPath:@"capturingStillImage"];
	[stillImageOutput release];
    
    
        
}



- (void)dealloc {
    [previousSecondTimestamps release];
    
    [super dealloc];
}

// perform a flash bulb animation using KVO to monitor the value of the capturingStillImage property of the AVCaptureStillImageOutput class
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if ( context == AVCaptureStillImageIsCapturingStillImageContext ) {
//		self.capturingStillImage = [[change objectForKey:NSKeyValueChangeNewKey] boolValue];
		
        NSLog(@"isCapturingStillImage: %i",[[change objectForKey:NSKeyValueChangeNewKey] boolValue]);
        
		
	}
}

- (void)captureSessionStoppedRunningNotification:(NSNotification *)notification
{
    
    //	dispatch_async(movieWritingQueue, ^{
    //		
    //	});
}



@end
