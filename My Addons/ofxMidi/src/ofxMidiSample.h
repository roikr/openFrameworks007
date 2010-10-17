/*
 *  ofxMidiSample.h
 *  Milgrom
 *
 *  Created by Roee Kremer on 10/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <deque>
#include "ofxSndFile.h"
using namespace std;

struct instance {
	int block;
	float volume;
	bool bStop;
};

class ofxMidiSample {
	
	
public:
		
	bool loadSample(string filename,int blockLength);
	void trigger(int velocity,bool retrigger);
	void stop();
	
	void mixWithBlocks(float *left,float *right);
	void postProcess(); // to call after processing

	
	void exit();
	
	int getNumPlaying();
	
	
private:
	ofxSndFile sample;
	deque<instance > instances;
	int blockLength;
};