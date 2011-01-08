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


struct actor {
	int x,y;
	float scale;
	float degree;
	int midi;
	int base;
	float pan;
	float volume;
	ofxiVideoPlayer *player;
};

struct note {
	ofxiVideoPlayer *player;
	int base;
	float fine;
	float pan;
	float volume;
	vector<actor*> actors;
};


struct layer {
	float x;
	float y;
	float scale;
	float degree;
	ofxiTexture *texture;
};

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
	
	void calcAudio();

	
	bool			bSmooth;
	float 			counter;
	
	
	ofxPincher pincher;
	int animStart;
	
	//ofImage image;
	vector<layer> layers;
	ofxiTexture background;
	
	
	ofxMidiTrack song;
	
	float 	* lAudio;
	float   * rAudio;
	
	bool bSongPlaying;
	bool bSolo;
	actor *soloist;
	
	
	map<string,ofxiPhoneVideo*> videos;
	//map<int,actor*> notes; 
	vector<actor> actors;
	map<int,note> notes;
	

};


