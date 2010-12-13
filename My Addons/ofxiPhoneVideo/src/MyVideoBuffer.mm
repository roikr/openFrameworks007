/*

File: MyVideoBuffer.mm

Abstract: Initializes the capture session by creating a video device 
          and input from the available video device. Creates the video
		  output for the capture session which is used to process 
		  uncompressed frames from the video device being captured. 
		  Creates an OpenGL texture to hold the video output data.
		  Implements the AVCaptureVideoDataOutputSampleBufferDelegate
		  protocol. The delegate is called when a sample buffer 
		  containing an uncompressed frame from the video being captured
		  is written. When a frame is written, it is copied to an OpenGL
		  texture for display.

Version: 1.0

Disclaimer: IMPORTANT:  This Apple software is supplied to you by 
Apple Inc. ("Apple") in consideration of your agreement to the
following terms, and your use, installation, modification or
redistribution of this Apple software constitutes acceptance of these
terms.  If you do not agree with these terms, please do not use,
install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and
subject to these terms, Apple grants you a personal, non-exclusive
license, under Apple's copyrights in this original Apple software (the
"Apple Software"), to use, reproduce, modify and redistribute the Apple
Software, with or without modifications, in source and/or binary forms;
provided that if you redistribute the Apple Software in its entirety and
without modifications, you must retain this notice and the following
text and disclaimers in all such redistributions of the Apple Software. 
Neither the name, trademarks, service marks or logos of Apple Inc. 
may be used to endorse or promote products derived from the Apple
Software without specific prior written permission from Apple.  Except
as expressly stated in this notice, no other rights or licenses, express
or implied, are granted by Apple herein, including but not limited to
any patent rights that may be infringed by your derivative works or by
other works in which the Apple Software may be incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2010 Apple Inc. All Rights Reserved.

*/

#import "MyVideoBuffer.h"
#import <MobileCoreServices/MobileCoreServices.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <CoreGraphics/CoreGraphics.h>

#import "ofxiPhoneVideo.h"


@interface MyVideoBuffer (Internal)
- (AVCaptureDevice *) cameraWithPosition:(AVCaptureDevicePosition) position;
- (AVCaptureDevice *) frontFacingCamera;
- (AVCaptureDevice *) backFacingCamera;

@end

@implementation MyVideoBuffer

@synthesize _session;
@synthesize previousTimestamp;
@synthesize videoFrameRate;
@synthesize videoDimensions;
@synthesize videoType;
@synthesize CameraTexture=m_textureHandle;

@synthesize videoInput = _videoInput;

-(id) initWithVideo:(ofxiPhoneVideo *)theVideo
{
	if ((self = [super init]))
	{
		video = theVideo;
		
		NSError * error;
		
		//-- Setup our Capture Session.
		self._session = [[AVCaptureSession alloc] init];
		
		[self._session beginConfiguration];
		
		//-- Set a preset session size. 
		[self._session setSessionPreset:AVCaptureSessionPresetMedium]; // AVCaptureSessionPresetLow AVCaptureSessionPreset640x480
		
		//-- Creata a video device and input from that Device.  Add the input to the capture session.
		AVCaptureDevice * videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
		if(videoDevice == nil)
			return nil;
		
		//-- Add the device to the session.
		AVCaptureDeviceInput *videoInput = [[[AVCaptureDeviceInput alloc] initWithDevice:[self backFacingCamera] error:&error] autorelease];
		[self setVideoInput:videoInput];
//		AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
//		if(error)
//			return nil;
		
		
		[self._session addInput:videoInput];
		
		//-- Create the output for the capture session.  We want 32bit BRGA
		AVCaptureVideoDataOutput * dataOutput = [[[AVCaptureVideoDataOutput alloc] init] autorelease];
		
		dataOutput.minFrameDuration = CMTimeMake(1, video->fps);
		
		
		[dataOutput setAlwaysDiscardsLateVideoFrames:YES]; // Probably want to set this to NO when we're recording
		
		// If you wish to cap the frame rate to a known value, such as 15 fps, set 
		// minFrameDuration.
		dataOutput.minFrameDuration = CMTimeMake(1, video->fps);
		
		[dataOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]]; // Necessary for manual preview
	
		// we want our dispatch to be on the main thread so OpenGL can do things with the data
		[dataOutput setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
		
		[self._session addOutput:dataOutput];
		
//		for ( AVCaptureConnection *connection in dataOutput.connections ) {
//			NSLog(@"support orientation: %i, orientation: %i",connection.supportsVideoOrientation,connection.videoOrientation);
//			if (connection.supportsVideoOrientation) {
//				connection.videoOrientation = AVCaptureVideoOrientationLandscapeRight;
//				NSLog(@"set orientation: %i",connection.videoOrientation);
//			}
//		}
		
		[self._session commitConfiguration];
		
		//-- Pre create our texture, instead of inside of CaptureOutput.
		//m_textureHandle = [self createVideoTextuerUsingWidth:1280 Height:720];
		//m_textureHandle = [self createVideoTextuerUsingWidth:640 Height:480];
		
		bIsCapturing = false;
		
	}
	return self;
}

- (BOOL) cameraToggle
{
    BOOL success = NO;
    
    if ([[AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo] count] > 1) {
        NSError *error;
        AVCaptureDeviceInput *videoInput = [self videoInput];
        AVCaptureDeviceInput *newVideoInput;
        AVCaptureDevicePosition position = [[videoInput device] position];
//        BOOL mirror;
        if (position == AVCaptureDevicePositionBack) {
            newVideoInput = [[AVCaptureDeviceInput alloc] initWithDevice:[self frontFacingCamera] error:&error];
            
        } else if (position == AVCaptureDevicePositionFront) {
            newVideoInput = [[AVCaptureDeviceInput alloc] initWithDevice:[self backFacingCamera] error:&error];
                   } else {
            goto bail;
        }
        
        //AVCaptureSession *session = [self session];
        if (newVideoInput != nil) {
            [self._session beginConfiguration];
            [self._session removeInput:videoInput];
            NSString *currentPreset = [self._session sessionPreset];
            if (![[newVideoInput device] supportsAVCaptureSessionPreset:currentPreset]) {
                [self._session setSessionPreset:AVCaptureSessionPresetHigh]; // default back to high, since this will always work regardless of the camera
            }
            if ([self._session canAddInput:newVideoInput]) {
                [self._session addInput:newVideoInput];
               // AVCaptureConnection *connection = [AVCamCaptureManager connectionWithMediaType:AVMediaTypeVideo fromConnections:[[self movieFileOutput] connections]];
//                if ([connection isVideoMirroringSupported]) {
//                    [connection setVideoMirrored:mirror];
//                }
                [self setVideoInput:newVideoInput];
            } else {
                [self._session setSessionPreset:currentPreset];
                [self._session addInput:videoInput];
            }
            [self._session commitConfiguration];
            success = YES;
            [newVideoInput release];
        } 
		
//		else if (error) {
//            id delegate = [self delegate];
//            if ([delegate respondsToSelector:@selector(someOtherError:)]) {
//                [delegate someOtherError:error];
//            }
//        }
    }
    
bail:
    return success;
}


- (AVCaptureDevice *) cameraWithPosition:(AVCaptureDevicePosition) position
{
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in devices) {
        if ([device position] == position) {
            return device;
        }
    }
    return nil;
}


- (AVCaptureDevice *) frontFacingCamera
{
    return [self cameraWithPosition:AVCaptureDevicePositionFront];
}

- (AVCaptureDevice *) backFacingCamera
{
    return [self cameraWithPosition:AVCaptureDevicePositionBack];
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

- (void) resetWithSize:(GLuint)w Height:(GLuint)h
{
	NSLog(@"_session beginConfiguration");
	[_session beginConfiguration];
	
	//-- Match the wxh with a preset.
	if(w == 1280 && h == 720)
	{
		[_session setSessionPreset:AVCaptureSessionPreset1280x720];
	}
	else if(w == 640)
	{
		[_session setSessionPreset:AVCaptureSessionPreset640x480];
	}
	else if(w == 480)
	{
		[_session setSessionPreset:AVCaptureSessionPresetMedium];
	}
	else if(w == 192)
	{
		[_session setSessionPreset:AVCaptureSessionPresetLow];
	}
	
	[_session commitConfiguration];
	NSLog(@"_session commitConfiguration");
}


- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
	CMTime timestamp = CMSampleBufferGetPresentationTimeStamp( sampleBuffer );
	if (CMTIME_IS_VALID( self.previousTimestamp ))
		self.videoFrameRate = 1.0 / CMTimeGetSeconds( CMTimeSubtract( timestamp, self.previousTimestamp ) );
	
	previousTimestamp = timestamp;

	CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);
	if (!m_textureHandle ) {
		
		self.videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
		NSLog(@"videoDimensions: %i %i",self.videoDimensions.width,self.videoDimensions.height);
		video->width = videoDimensions.width;
		video->height = videoDimensions.height;
		//-- If we haven't created the video texture, do so now.
		m_textureHandle = [self createVideoTextuerUsingWidth:videoDimensions.width Height:videoDimensions.height];
		
		for (int i=0; i<video->numFrames; i++) {
			video->textures.push_back([self createVideoTextuerUsingWidth:videoDimensions.width Height:videoDimensions.height]);
		}
	}
	
	
	CMVideoCodecType type = CMFormatDescriptionGetMediaSubType(formatDesc);
#if defined(__LITTLE_ENDIAN__)
	type = OSSwapInt32(type);
#endif
	self.videoType = type;
	
	CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
	CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
	
	
	
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);
	
	unsigned char* linebase = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
	
	
	if (bIsCapturing ) {
		glBindTexture(GL_TEXTURE_2D, video->textures[currentFrame % video->numFrames]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
		currentFrame++;
		
		if (bIsRecording) {
			if ( currentFrame - firstFrame >= video->numFrames - video->numIntroFrames)  {
				bIsCapturing = false;
			}
		}
	}
	
	CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
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
	
	float w = videoDimensions.width;
	float h = videoDimensions.height;
	
	GLfloat spriteVertices[] =  {
		w,h,0,   
		w,0,0,   
		0,h,0, 
		0,0,0};
	
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

- (void) capture {
	bIsCapturing = true;
	bIsRecording = false;
	currentFrame = 0;
}



- (void) record {
	if (!m_textureHandle)
		return;
	
	bIsRecording = true;
	firstFrame = currentFrame-1;
	video->firstFrame = (currentFrame-1) % video->numFrames;
	
	
		
}



- (void)dealloc 
{
	[_session release];
	
	[super dealloc];
}

@end