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

class ofxiVideoGrabber {
	
public:
	
	//ofxiPhoneClipper();
	//~ofxiPhoneClipper();
	
	void setup(ofxiPhoneVideo *video);
	void update();
	void draw();
	void drawAudio();
	void exit();
	
	void startSession();
	void stopSession();
	bool getIsSessionRunning();
	void startRecording();
	void stopRecording();
	bool getIsRecording();
	
	void audioReceived( float * input, int bufferSize);
	
protected:
	
	
	//int		drawCounter, bufferCounter;
	int		currentBuffer;
	
	bool bIsSessionRunning;
	bool bIsRecording;
	
	MyVideoBuffer *	videoTexture;
	int start;
	
	
	float *buffer;
	
	deque<float> scaledSamples;
	
	ofxiPhoneVideo *video;
	
};
