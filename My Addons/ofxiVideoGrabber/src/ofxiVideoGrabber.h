/*
 *  ofxiVideoGrabber.h
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include <vector>
#include <deque>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include "ofxiFBO.h"



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
	void render();
	void draw();
	void drawLiveCam();
	void drawTexture(int texture);
	
	void exit();
		
	void startCamera();
	void stopCamera();
	
	void startCapture();
	void record();
	void pause();
	
	int getState();
	
//	void audioReceived( float * input, int bufferSize);
	
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
	
	int cameraFrame;
	
	ofxiFBO fbo;
	
};
