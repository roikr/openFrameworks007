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


using namespace std;

@class MyVideoBuffer;
class ofxiPhoneVideo;

enum {
	CAMERA_IDLE,
	CAMERA_RUNNING,
	CAMERA_CAPTURING
};

class ofxiVideoGrabber {
	
public:
	
	ofxiVideoGrabber();
	//~ofxiPhoneClipper();
	
	void setup(ofxiPhoneVideo *video);
	void update();
	void draw();
	
	void exit();
	
	void setTrigger(float thresh);
	bool getIsTriggered();
	float getMeter();
	
	void startCamera();
	void stopCamera();
	
	void startCapture();
	void record();
	
	int getState();
	
	void audioReceived( float * input, int bufferSize);
	
	
protected:
	
	
	//int		drawCounter, bufferCounter;
	//int		currentBuffer;
	
	MyVideoBuffer *	videoTexture;
	
	//float *buffer;
	
	//deque<float> scaledSamples;
	
	int state;
	
	ofxiPhoneVideo *video;
	
};
