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
#include "ofMain.h"



using namespace std;

@class MyVideoBuffer;
class ofxiPhoneVideo;

enum {
	FRONT_CAMERA,
	BACK_CAMERA
};

enum {
    CAMERA_NONE,
    CAMERA_READY,
    CAMERA_STARTED,
	CAMERA_RUNNING,
	CAMERA_CAPTURING,
	CAMERA_RECORDING
};

class ofxiVideoGrabber {
	
public:
	
	ofxiVideoGrabber();
	//~ofxiPhoneClipper();
	
	void setup(ofxiPhoneVideo *video,int cameraPosition);
    void setTransform(ofPoint &offset,float scale);
    
	void update();
	void render();
	void draw();
	
	void exit();
	void suspend();
		
	void startCamera();
	void stopCamera();
	void startCapture();
	void stopCapture();
	void record();
	
	
	int getState();
	
//	void audioReceived( float * input, int bufferSize);
	
	bool cameraToggle();
//	void setCameraPosition(int cameraPosition);
	
	int getCameraWidth();
	int getCameraHeight();
	
	void drawCamera();
	
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
	
	ofPoint offset;
	float scale;
	
    bool bStartCapture;
	
		
};
