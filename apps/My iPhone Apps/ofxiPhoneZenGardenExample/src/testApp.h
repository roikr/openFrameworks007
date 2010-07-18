#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ZenGarden.h"

class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	void touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	void touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	void touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data);
	
	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	
	void audioRequested 	(float * input, int bufferSize, int nChannels);
	
	
	
	
	float 	volume;
	
	float *inputBuffers;
	float *outputBuffers;
	
	PdGraph *graph;
	
	static const int blockSize = 64;
	static const int numInputChannels = 2;
	static const int numOutputChannels = 2;
	static const float sampleRate = 22050.0f;
	//const float sampleRate = 44100.0f;	
	
	bool bZGInitialized;
	int blockIndex;
};

