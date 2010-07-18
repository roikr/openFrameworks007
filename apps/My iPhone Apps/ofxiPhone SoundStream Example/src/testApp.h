#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"

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
	
	float 	pan;
	int		sampleRate;
	bool 	bNoise;
	float 	volume;
	
	float 	* lAudio;
	float   * rAudio;
	
	//------------------- for the simple sine wave synthesis
	float 	targetFrequency;
	float 	phase;
	float 	phaseAdder;
	float 	phaseAdderTarget;
	
};

