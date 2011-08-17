/*
 *  ofxAudioInstrument.h
 *  MusicCreed
 *
 *  Created by Roee Kremer on 3/28/11.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;






class ofxAudioFile;

struct note {
	int midi;
	int velocity;
	int cents;
	ofxAudioFile *sample;
};

class ofxAudioInstrument { 
	
	
public:
	
	void setup(int blockLength,int sampleTriggers); // call before loading samples loaded 
	
	void loadSample(string filename,int midi,bool bChokeGroup = false);
	void noteOn(int midi,int velocity,int cents=0);
	void noteOff(int midi);
	void noteOffAll();
	
	void preProcess();
	//void mixWithBlocks(float *left,float *right);
	void mixChannel(float * output, int channel, int nChannels);

	void postProcess(); // to call after each step to process next
	
	void exit();
	
	bool getIsPlaying();
	
	
private:
	map<int,ofxAudioFile*> samples;
	set<int> chokeGroup;
	vector<note> start;
	vector<note> playing;
	vector<int> stop;
	
		
	int blockLength;
	
	
	bool bNoteOffAll;
	int sampleTriggers;
		
};
	
	

	


