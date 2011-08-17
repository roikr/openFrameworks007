/*
 *  ofxAudioSample.h
 *  audioVideoGrabber
 *
 *  Created by Roee Kremer on 8/17/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <vector>


// for now supporting 44100 mono audio

using namespace std;

struct ofxAudioSample {
	float *buffer;
	int nChannels;
	int numFrames;
};
	