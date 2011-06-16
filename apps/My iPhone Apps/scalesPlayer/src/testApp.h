#pragma once


#include "ofMain.h"
#include "ofxAudioInstrument.h"
#include "ofxAudioFile.h"

#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#define SCALES_PLAYER_INSTRUMENT

class testApp : public ofxiPhoneApp{
	
	public:


		void setup();
		void update();
		void draw();
	void exit();

		

		void audioRequested 	(float * input, int bufferSize, int nChannels);
	
	ofxAudioInstrument instrument;
	ofxAudioFile sample;
		
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

	int lastKey;
	int lastArp;
	
	
};

