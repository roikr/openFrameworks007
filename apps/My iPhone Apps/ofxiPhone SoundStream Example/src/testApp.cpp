
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	// touch events will be sent to testApp
	ofxMultiTouch.addListener(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	ofBackground(255,255,255);
	
	pan = 0.5f;
	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.5f;
	bNoise 				= false;
	lAudio = new float[256];
	rAudio = new float[256];
	
	targetFrequency = 2000.0f;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
	
	ofSoundStreamSetup(2,0,this, sampleRate,256, 4);
	
	ofSetFrameRate(60);
}


//--------------------------------------------------------------
void testApp::update() {

}

//--------------------------------------------------------------
void testApp::draw() {

}

void testApp::exit() {
}


//--------------------------------------------------------------
void testApp::touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data){
}

//--------------------------------------------------------------
void testApp::lostFocus() {
}

//--------------------------------------------------------------
void testApp::gotFocus() {
}

//--------------------------------------------------------------
void testApp::gotMemoryWarning() {
}

void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
	//
	float leftScale = 1 - pan;
	float rightScale = pan;
	
	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}
	
	phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
	for (int i = 0; i < bufferSize; i++){
		phase += phaseAdder;
		float sample = sin(phase);
		lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
		rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
	}
	
	
}