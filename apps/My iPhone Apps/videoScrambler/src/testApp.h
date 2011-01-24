#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"


#include "ofxiVideoStreamer.h"



class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	
	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	
	void audioRequested( float * output, int bufferSize, int nChannels );
	
		
	ofxiVideoStreamer streamer;
	
	ofPoint pos;
	ofPoint down;

};


