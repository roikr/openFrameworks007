/*
 *  ofxiVideoStreamer.h
 *  kobiPhone
 *
 *  Created by Roee Kremer on 1/23/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include "ofMain.h"

using namespace std;

@class VideoReader;

class ofxiVideoStreamer {
	
public:
	

	ofxiVideoStreamer() : bStreaming(false) {};
	void setup(string filename);
	void update();
	void draw(ofRectangle rect,ofRectangle tex);
	void exit();
	
	
	void audioRequested( float * output, int bufferSize, int nChannels );
	float getFrameRate();
	
	float getWidth();
	float getHeight();

	bool getIsStreaming();
	int getTexture();
	int getCurrentFrame();
	int getElapsedFrame();
	
	
protected:
	
	VideoReader *	videoReader;
	
	int playPos;
	
	int startTime;
	int currentFrame;
	bool bStreaming;
	
};