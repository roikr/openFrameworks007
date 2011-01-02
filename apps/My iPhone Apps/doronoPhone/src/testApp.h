#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxPincher.h"
#include "ofxiTexture.h"
#include "ofxiPhoneVideo.h"
#include "ofxiVideoLoader.h"
#include "ofxiVideoPlayer.h"
#include "ofxMidiTrack.h"

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

	
	bool			bSmooth;
	float 			counter;
	
	
	ofxPincher pincher;
	int animStart;
	
	//ofImage image;
	ofxiTexture texture;
	
	ofxiPhoneVideo video;
	ofxiVideoLoader loader;
	ofxiVideoPlayer player;
	ofxMidiTrack song;
	
	float 	* lAudio;
	float   * rAudio;
	
	bool bSongPlaying;
	

};


