//
//  VideoStream.m
//  videoStream
//
//  Created by Roee Kremer on 2/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "VideoStream.h"
#import <CoreVideo/CoreVideo.h>
#import <CoreVideo/CVOpenGLESTextureCache.h>


@interface VideoStream () {
    
    
    NSString *_sessionPreset;
    
    AVCaptureSession *_session;
    
}




@end

@implementation VideoStream


- (void)captureOutput:(AVCaptureOutput *)captureOutput 
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer 
       fromConnection:(AVCaptureConnection *)connection
{
    CVReturn err;
    
    CMFormatDescriptionRef desc = CMSampleBufferGetFormatDescription (sampleBuffer);
     
    CMMediaType mediaType = CMFormatDescriptionGetMediaType (desc);
    
    NSLog(@"%@",mediaType);
    
}



- (void)setupAVCapture
{
   
//    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
//    {
//        
//        // Choosing bigger preset for bigger screen.
//        _sessionPreset = AVCaptureSessionPreset1280x720;
//    }
//    else
//    {
//        
//        _sessionPreset = AVCaptureSessionPreset640x480;        
//    } 

    _sessionPreset = AVCaptureSessionPreset352x288;
    
    //-- Setup Capture Session.
    _session = [[AVCaptureSession alloc] init];
    [_session beginConfiguration];
    
    //-- Set preset session size.
    [_session setSessionPreset:_sessionPreset];
    
    //-- Creata a video device and input from that Device.  Add the input to the capture session.
    AVCaptureDevice * videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    if(videoDevice == nil)
        assert(0);
    
    //-- Add the device to the session.
    NSError *error;        
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
    if(error)
        assert(0);
    
    [_session addInput:input];
    
    //-- Create the output for the capture session.
    AVCaptureVideoDataOutput * dataOutput = [[AVCaptureVideoDataOutput alloc] init];
    
   
    
    [dataOutput setAlwaysDiscardsLateVideoFrames:YES]; // Probably want to set this to NO when recording
    
    
    
    /*
    //-- Set to YUV420.
    [dataOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange] 
                                                             forKey:(id)kCVPixelBufferPixelFormatTypeKey]]; // Necessary for manual preview
    */
    
    //[dataOutput setVideoSettings:[NSDictionary dictionaryWithObject:AVVideoCodecH264 forKey:AVVideoCodecKey]];
    [dataOutput setVideoSettings:[NSDictionary dictionary]];
    
    
    NSArray *codecs = [dataOutput availableVideoCodecTypes];
    NSLog(@"availableVideoCodecTypes");
    for (NSString *codec in codecs) {
        NSLog(@"%@",codec);
    }
    
    // Set dispatch to be on the main thread so OpenGL can do things with the data
    [dataOutput setSampleBufferDelegate:self queue:dispatch_get_main_queue()];        
    
    [_session addOutput:dataOutput];
    [_session commitConfiguration];
    
    
    
    //[_session startRunning];
}

- (void)tearDownAVCapture
{
    [_session stopRunning];
}

@end
