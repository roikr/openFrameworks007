#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxiPhoneVideo.h"
#include "ofxiVideoLoader.h"
#include "ofxiVideoPlayer.h"




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
	
	void audioRequested( float * output, int bufferSize, int nChannels );

	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
	
	ofxiPhoneVideo video;
	ofxiVideoLoader loader;
	ofxiVideoPlayer player;
	
	float 	* lAudio;
	float   * rAudio;

};


