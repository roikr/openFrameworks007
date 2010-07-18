#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "sndfile.h"


#define	BUFFER_LENGTH		(1 << 8)


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
	
	double buf[BUFFER_LENGTH];
	SNDFILE * sf;
	sf_count_t len;
	SF_INFO sfinfo;
	
};

