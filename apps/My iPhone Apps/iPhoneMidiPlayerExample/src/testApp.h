#pragma once


#include "ofMain.h"
#include "ofxMidiInstrument.h"
#include "ofxMidiLooper.h"

#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

class testApp : public ofxiPhoneApp{
	
	public:


		void setup();
		void update();
		void draw();
	void exit();

		

		void audioRequested 	(float * input, int bufferSize, int nChannels);
	
	ofxMidiInstrument instrument;
	ofxMidiLooper looper;
		
		void touchDown(ofTouchEventArgs &touch);
		void touchMoved(ofTouchEventArgs &touch);
		void touchUp(ofTouchEventArgs &touch);
		void touchDoubleTap(ofTouchEventArgs &touch);

		float 	pan;
		int		sampleRate;
		
		float 	volume;

		float 	* lAudio;
		float   * rAudio;
	
	float *lBlock;
	float *rBlock;

	
	int bpm;
	
};

