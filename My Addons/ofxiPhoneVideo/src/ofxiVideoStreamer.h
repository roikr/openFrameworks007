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

using namespace std;

@class VideoReader;

class ofxiVideoStreamer {
	
public:
	

	
	void setup(string filename);
	void update();
	void draw();
	void exit();
	
	
	void audioRequested( float * output, int bufferSize, int nChannels );
	float getFrameRate();

	
protected:
	
	VideoReader *	videoReader;
	
	int playPos;
	
};