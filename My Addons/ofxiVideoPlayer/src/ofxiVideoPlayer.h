//
//  ofxiVideoPlayer.h
//  videoPlayerExample
//
//  Created by Roee Kremer on 5/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef videoPlayerExample_ofxiVideoPlayer_h
#define videoPlayerExample_ofxiVideoPlayer_h

#pragma once

#include <string>
#include "ofMain.h"

using namespace std;

@class VideoPlayer;

class ofxiVideoPlayer {
	
public:
	ofxiVideoPlayer():bAllocated(false) {};
	void play(string filename);
	void update();
	void draw(ofRectangle rect,ofRectangle tex=ofRectangle(0,0,1,1));
	void exit();
	
    void stop();
    
	void audioRequested( float * output, int bufferSize, int nChannels );
	
    float getFrameRate();
	
	float getWidth();
	float getHeight();
    
	bool getIsPlaying();
    bool getIsFrameVisible();
	int getCurrentFrame();
	int getElapsedFrame();
	
	
protected:
    bool bAllocated;
	
	VideoPlayer *	videoPlayer;
	
	int playPos;
	
	int startTime;
	int currentFrame;
	
    bool bPlaying;
	
};

#endif
