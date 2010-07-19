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
	
	void clear();
	void setup(int blockLength=256,int sampleRate=44100,int bpm=120); 	
	void loadLoop(string filename);
	
	void sync(); // sync to other loops
	
	void play();
	void pause();
	
	void playLoop(int i);
	
	void process(vector<event> &events);
	
	void stopLoop();
	
	void setBPM(int bpm);
	
	
private:
	
	int sampleRate;
	int blockLength;
	
	int blockIndex;
	int startBlock;
	
	int startTick;
	
	vector<loop> loops;
	
	
	bool bPlaying;
	
	int bpm;
	float ticksPerBlock;
	int nextBPM;
	bool bChangeBPM;
	
	bool bNewLoop;
	int bLoopPlaying;
	
	int currentLoop;
	int nextLoop;
	
};