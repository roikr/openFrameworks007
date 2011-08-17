#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#include "ofxiVideoGrabber.h"
#include "ofxiVideoPlayer.h"
#include "ofxiPhoneVideo.h"
#include "ofxAudioSampler.h"
#include "ofxAudioPlayer.h"

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

	float 			counter;
	ofTrueTypeFont 	vagRounded;
	char eventString[255];
	char timeString[255];

	
	ofxiPhoneVideo video;
	ofxiVideoGrabber grabber;
	ofxiVideoPlayer videoPlayer;
	ofxAudioSampler sampler;
	ofxAudioPlayer audioPlayer;
	
	bool bRecording;
};


