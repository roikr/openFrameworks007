#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#include "ofxiPhoneVideo.h"
#include "ofxiVideoGrabber.h"
#include "ofxiVideoPlayer.h"

struct playerLayout {
	int x;
	int y;
	float scale;
	int degree;
};


class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	
	void audioReceived( float * input, int bufferSize, int nChannels );
	void audioRequested( float * output, int bufferSize, int nChannels );
	
	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
	
	ofxiPhoneVideo video;
	ofxiVideoGrabber camera;
	ofxiVideoPlayer player[4];
	
	float *buffer;
	
	vector<playerLayout> layouts;

};

