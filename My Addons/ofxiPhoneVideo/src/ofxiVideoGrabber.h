/*
 *  ofxiVideoGrabber.h
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>
#include <deque>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>



using namespace std;

@class MyVideoBuffer;
class ofxiPhoneVideo;

enum {
	CAMERA_IDLE,
	CAMERA_RUNNING,
	CAMERA_CAPTURING,
	CAMERA_RECORDING
};

class ofxiVideoGrabber {
	
public:
	
	ofxiVideoGrabber();
	//~ofxiPhoneClipper();
	
	void setup(ofxiPhoneVideo *video);
	void update();
	void fboDraw();
	void draw();
	void drawTexture(int text,int width,int height);
	
	void exit();
		
	void startCamera();
	void stopCamera();
	
	void startCapture();
	void record();
	
	int getState();
	
	void audioReceived( float * input, int bufferSize);
	
	bool cameraToggle();
	
protected:
	
	
	//int		drawCounter, bufferCounter;
	//int		currentBuffer;
	
	MyVideoBuffer *	videoTexture;
	
	//float *buffer;
	
	//deque<float> scaledSamples;
	
	int state;
	
	ofxiPhoneVideo *video;
	
	int currentFrame;
	int firstFrame;
	
	GLuint fbo;
	
	int cameraFrame;
	
};
