/*
 *  ofxiVideoPlayer.h
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


class ofxiPhoneVideo;

class ofxiVideoPlayer {
	
public:
	
	//ofxiPhoneClipper();
	//~ofxiPhoneClipper();
	
	void setup(ofxiPhoneVideo *video,bool bIntroMode = false);
	void update();
	void draw();
	void exit();
	
	void play(int note,int velocity);
	void play(float speed,float volume=1.0f);
	bool getIsPlaying();
	
	void updateFrame();
	
//	void firstFrame();

	
	void introFrame();
	void playIntro();
	
		
	
	
//	void startScrubbing();
//	void stopScrubbing();
//	bool getIsScrubbing();
//	void setPosition(float pos);
	
	void audioRequested( float * output, int bufferSize);
	void mix(float *buffer,int bufferSize,float volume=1.0f);
	
	void preProcess(); // should be call after audioRequested or mix to update position and state
	
	

	
	
	
protected:
	
	void seekFrame(int nextFrame);
	
	//int		drawCounter, bufferCounter;

	int start;
	int currentTexture;
	int state;
	
	float speed;
	float volume;
	int pos;
	
	ofxiPhoneVideo *video;
	
	int currentFrame;
	
	bool bIntroMode;
	
};
