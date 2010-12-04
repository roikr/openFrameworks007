
#import "MyBuffer.h"
#import <MobileCoreServices/MobileCoreServices.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <CoreGraphics/CoreGraphics.h>
#include "ofMain.h"

#define SINT16_MAX 32767.0

@implementation MyBuffer

@synthesize _session;
@synthesize previousTimestamp;
@synthesize videoFrameRate;
@synthesize videoDimensions;
@synthesize videoType;
@synthesize CameraTexture=m_textureHandle;

-(id) init
{
	if ((self = [super init]))
	{
		NSError * error;
		
		//-- Setup our Capture Session.
		self._session = [[AVCaptureSession alloc] init];
		
		[self._session beginConfiguration];
		
		//-- Set a preset session size. 
		[self._session setSessionPreset:AVCaptureSessionPreset640x480];
		
		//-- Creata a video device and input from that Device.  Add the input to the capture session.
		AVCaptureDevice * videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
		if(videoDevice == nil)
			return nil;
		
		//-- Add the device to the session.
		AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
		if(error)
			return nil;
		
		[self._session addInput:input];
		[self addAudioInput];
		
		//-- Create the output for the capture session.  We want 32bit BRGA
		AVCaptureVideoDataOutput * dataOutput = [[[AVCaptureVideoDataOutput alloc] init] autorelease];
		[dataOutput setAlwaysDiscardsLateVideoFrames:YES]; // Probably want to set this to NO when we're recording
		[dataOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]]; // Necessary for manual preview
	
		// we want our dispatch to be on the main thread so OpenGL can do things with the data
		[dataOutput setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
		
		
		[self._session addOutput:dataOutput];
		[self addAudioDataOutput];
		[self._session commitConfiguration];
		
		//-- Pre create our texture, instead of inside of CaptureOutput.
		//m_textureHandle = [self createVideoTextuerUsingWidth:1280 Height:720];
		m_textureHandle = [self createVideoTextuerUsingWidth:640 Height:480];
		bRecording = false;
	}
	return self;
}

- (void) addAudioInput {
	
	AVCaptureDevice *audioDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
	if	(audioDevice) {
		
		NSError *error;
		AVCaptureDeviceInput *audioIn = [AVCaptureDeviceInput deviceInputWithDevice:audioDevice error:&error];
		if ( !error ) {
			if ([self._session canAddInput:audioIn])
				[self._session addInput:audioIn];
			else
				NSLog(@"Couldn't add audio input");		
		}
		else
			NSLog(@"Couldn't create audio input");
	}
	else
		NSLog(@"Couldn't create audio capture device");
}

- (void) addAudioDataOutput {
	
	AVCaptureAudioDataOutput *audioOut = [[AVCaptureAudioDataOutput alloc] init];
	[audioOut setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
	if ([self._session canAddOutput:audioOut]) {
		[self._session addOutput:audioOut];
		//audioConnection = [audioOut connectionWithMediaType:AVMediaTypeAudio];
	}
	else
		NSLog(@"Couldn't add audio output");
	[audioOut release];
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, textureData);

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
	if ([captureOutput isKindOfClass:[AVCaptureAudioDataOutput self]]) {
		[self captureAudioSampleBuffer:sampleBuffer];
	} else if ([captureOutput isKindOfClass:[AVCaptureVideoDataOutput self]]) {
		[self captureVideoSampleBuffer:sampleBuffer];
	}
}

- (void) captureVideoSampleBuffer:(CMSampleBufferRef)sampleBuffer {

	CMTime timestamp = CMSampleBufferGetPresentationTimeStamp( sampleBuffer );
	if (CMTIME_IS_VALID( self.previousTimestamp ))
		self.videoFrameRate = 1.0 / CMTimeGetSeconds( CMTimeSubtract( timestamp, self.previousTimestamp ) );
	
	previousTimestamp = timestamp;

	CMFormatDescriptionRef formatDesc = CMSampleBufferGetFormatDescription(sampleBuffer);
	self.videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDesc);
	
	CMVideoCodecType type = CMFormatDescriptionGetMediaSubType(formatDesc);
#if defined(__LITTLE_ENDIAN__)
	type = OSSwapInt32(type);
#endif
	self.videoType = type;
	
	CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
	CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
	
	//-- If we haven't created the video texture, do so now.
	if(m_textureHandle == 0)
	{
		m_textureHandle = [self createVideoTextuerUsingWidth:videoDimensions.width Height:videoDimensions.width];
	}
	
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);
	
	unsigned char* linebase = (unsigned char *)CVPixelBufferGetBaseAddress( pixelBuffer );
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
	
	
	if (bRecording) {
		glBindTexture(GL_TEXTURE_2D, *currentTexture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoDimensions.width, videoDimensions.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, linebase);
		currentTexture++;
		if (currentTexture==textures.end()) {
			bRecording = false;
		}
	}
	
	CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
}


// Returns array member with greatest absolute value
- (SInt16) maxValueInArray: (SInt16[]) values ofSize: (unsigned int) size {
	
	SInt16 max = 0;
	
	for (int i = 0; i < size; i++)
		if (abs(values[i]) > max)
			max = values[i];
	
	return max;
}


- (void) captureAudioSampleBuffer:(CMSampleBufferRef)sampleBuffer {
	
	CMItemCount numSamples = CMSampleBufferGetNumSamples(sampleBuffer);
	NSUInteger channelIndex = 0;
	
	CMBlockBufferRef audioBlockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
	size_t audioBlockBufferOffset = (channelIndex * numSamples * sizeof(SInt16));
	size_t lengthAtOffset = 0;
	size_t totalLength = 0;
	SInt16 *samples = NULL;
	CMBlockBufferGetDataPointer(audioBlockBuffer, audioBlockBufferOffset, &lengthAtOffset, &totalLength, (char **)(&samples));
	
	int numSamplesToRead = 1;
	for (int i = 0; i < numSamplesToRead; i++) {
		
		SInt16 subSet[numSamples / numSamplesToRead];
		for (int j = 0; j < numSamples / numSamplesToRead; j++)
			subSet[j] = samples[(i * (numSamples / numSamplesToRead)) + j];
		
		SInt16 lastAudioSample	 = [self maxValueInArray: subSet ofSize: numSamples / numSamplesToRead];
		
		double scaledSample = (double) (lastAudioSample / SINT16_MAX);
		
		//[audioDisplayDelegate addX:scaledSample];
		if (scaledSamples.size()>=ofGetWidth()) {
			scaledSamples.pop_back();
		}
		scaledSamples.push_front(scaledSample);
	}
}

-(void)renderCameraToSprite:(uint)text
{
	float vW=videoDimensions.width;
	float vH=videoDimensions.height;
//	float tW=1280;
//	float tH=720;
	float tW=640;
	float tH=480;
	
	GLfloat spriteTexcoords[] = {
		vW/tW,vH/tH,   
		vW/tW,0.0f,
		0,vH/tH,   
		0.0f,0,};
	
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
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	
}


- (void) setup:(uint)n {
	for (int i=0; i<n; i++) {
		textures.push_back([self createVideoTextuerUsingWidth:640 Height:480]);
	}
}


- (void) start {
	bRecording = true;
	currentTexture = textures.begin();
}

- (bool) isRecording {
	return bRecording;
}

- (uint) numberOfTextures {
	return textures.size();
}

- (uint) texture:(uint)i {
	return textures[i];
}

- (void) renderSamples {
	for (deque<double>::iterator iter = scaledSamples.begin(); iter!=scaledSamples.end(); iter++) {
		ofLine(distance(scaledSamples.begin(),iter), ofGetHeight()*(0.5+*iter*0.5), 
			   distance(scaledSamples.begin(),iter), ofGetHeight()*(0.5-*iter*0.5));
	}
}

- (void)dealloc 
{
	[_session release];
	
	[super dealloc];
}

@end
