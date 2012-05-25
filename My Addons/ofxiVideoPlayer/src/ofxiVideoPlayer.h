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
	
	void play(string filename);
	void update();
	void draw(ofRectangle rect,ofRectangle tex);
	void exit();
	
    void stop();
    
	void audioRequested( float * output, int bufferSize, int nChannels );
	
    float getFrameRate();
	
	float getWidth();
	float getHeight();
    
	bool getIsPlaying();
	int getCurrentFrame();
	int getElapsedFrame();
	
	
protected:
	
	VideoPlayer *	videoPlayer;
	
	int playPos;
	
	int startTime;
	int currentFrame;
	
    bool bPlaying;
	
};

#endif
