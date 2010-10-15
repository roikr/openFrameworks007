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



class ofxMidiSample {
	
	
public:
		
	bool loadSample(string filename,int blockLength);
	void trigger(int velocity);
	void stop();
	
	void mixWithBlocks(float *left,float *right);
	void postProcess(); // to call after processing

	
	void exit();
	
	int getNumPlaying();
	
	
private:
	ofxSndFile sample;
	deque<pair<int,float> > instances;
	int blockLength;
	bool bStop;
	
};