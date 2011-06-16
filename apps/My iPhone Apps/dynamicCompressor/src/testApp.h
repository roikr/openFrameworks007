#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxDynamicCompressor.h"
#include "ofxAudioTrigger.h"
#include "ofxAudioSampler.h"

class testApp : public ofxiPhoneApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	
	void audioReceived(float * input, int bufferSize, int nChannels);
	void audioRequested(float * output, int bufferSize, int nChannels);


	int		bufferSize;
	int		sampleRate;
	int		drawCounter, bufferCounter;
	float	* buffer;
	
	float	* amplitude;
	int		currentBuffer;
	
	float 	pan;
	float 	volume;
	
	ofxAudioTrigger trigger;
	ofxAudioSampler sampler;
	ofxDynamicCompressor compressor;
	
	float scale;
	bool bDrag;
	int lastX;
	
};

