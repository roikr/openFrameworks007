/*
 *  ofxMidiLooper.h
 *  midiPlayerExample
 *
 *  Created by Roee Kremer on 7/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "ofxMidi.h"


struct loop {
	ofxMidi *midiFile;
	int numTicks;
};

class ofxMidiLooper {
	
	
public:
	
	
	void setup(int blockLength,int sampleRate=44100); // call after all samples loaded 
		
	void loadLoop(string filename);
	void start();
	
	void stop();
	void playLoop(int i);
	
	void process(vector<event> &events);
	
	void stopLoop();
	
	void setBPM(int bpm);
	
	
	

	void exit() {}; //TODO: implement exit
	
	
private:
	
	int sampleRate;
	int blockLength;
	
	int blockIndex;
	int startBlock;
	
	int startTick;
	
	vector<loop> loops;
	
	bool bStarted;
	
	int bpm;
	float ticksPerBlock;
	int nextBPM;
	bool bChangeBPM;
	
	bool bNewLoop;
	int bLoopPlaying;
	
	int currentLoop;
	int nextLoop;
	
};