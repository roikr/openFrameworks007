/*
 *  ofxAudioSample.h
 *  MusicCreed
 *
 *  Created by Roee Kremer on 3/28/11.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <deque>

using namespace std;

struct instance {
	int block;
	float volume;
	bool bStop;
};

class ofxAudioSample;

class ofxSamplePlayer {
	
	
public:
		
	void setup(ofxAudioSample *sample,int blockLength);
	void trigger(int velocity,bool retrigger);
	void stop();
	
	//void mixWithBlocks(float *left,float *right);
	void mixChannel(float * output, int channel, int nChannels);

	void postProcess(); // to call after processing

	
	void exit();
	
	int getNumPlaying();
	
	
private:
	ofxAudioSample *sample;
	deque<instance > instances;
	int blockLength;
};