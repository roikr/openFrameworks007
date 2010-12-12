/*
 *  ofxiVideoPlayer.h
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>
#include <deque>

using namespace std;

class ofxiPhoneVideo;

class ofxiVideoPlayer {
	
public:
	
	//ofxiPhoneClipper();
	//~ofxiPhoneClipper();
	
	void setup(ofxiPhoneVideo *video);
	void update();
	void draw();
	void exit();
	
	void play(float speed);
	bool getIsPlaying();
	
	void introFrame();
	
	void firstFrame();
	void nextFrame();
	
//	void startScrubbing();
//	void stopScrubbing();
//	bool getIsScrubbing();
//	void setPosition(float pos);
	
	void audioRequested( float * output, int bufferSize);
	void mix(float *buffer,int bufferSize,float volume=1.0f);
	
	void drawFrame(int frame);

	
	
	
protected:
	
	void drawTexture(int texture);
	
	//int		drawCounter, bufferCounter;

	int start;
	int currentTexture;
	int state;
	
	float speed;
	int pos;
	
	ofxiPhoneVideo *video;
	
	int currentFrame;
	
};
