
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include <OpenGLES/ES1/glext.h>
#include <vector>
#include <deque>

using namespace std;
									 
@interface MyBuffer : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate,AVCaptureAudioDataOutputSampleBufferDelegate>{

	AVCaptureSession*		_session;

	CMTime previousTimestamp;
	
	Float64 videoFrameRate;
	CMVideoDimensions videoDimensions;
	CMVideoCodecType videoType;
	
	uint m_textureHandle;
	vector<uint> textures;
	vector<uint>::iterator currentTexture;
	bool bRecording;
	
	deque<double> scaledSamples;
	
	
	
}

@property (nonatomic, retain) AVCaptureSession* _session;
@property (readwrite) Float64 videoFrameRate;
@property (readwrite) CMVideoDimensions videoDimensions;
@property (readwrite) CMVideoCodecType videoType;
@property (readwrite) CMTime previousTimestamp;

@property (readwrite) uint CameraTexture;



- (void)	captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;

- (void) captureVideoSampleBuffer:(CMSampleBufferRef)sampleBuffer;
- (void)	renderCameraToSprite:(uint)text;
- (GLuint)	createVideoTextuerUsingWidth:(GLuint)w Height:(GLuint)h;
- (void)	resetWithSize:(GLuint)w Height:(GLuint)h;
- (void) setup:(uint)n;
- (void) start;
- (bool) isRecording;
- (uint) numberOfTextures;
- (uint) texture:(uint)i;

- (void) addAudioInput;
- (void) addAudioDataOutput;
- (void) captureAudioSampleBuffer:(CMSampleBufferRef)sampleBuffer;
- (void) renderSamples;

@end
