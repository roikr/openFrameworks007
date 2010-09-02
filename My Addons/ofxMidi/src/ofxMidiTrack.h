/*
 *  ofxMidiTrack.h
 *  midiPlayerExample
 *
 *  Created by Roee Kremer on 7/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "ofxMidi.h"


class ofxMidiTrack {
	
	
public:
	
	//void clear();
	void setup(int blockLength=256,int sampleRate=44100,int bpm=120); 	
	void loadTrack(string filename);
	void saveTrack(string filename);
	
	void play();
	void record();
	void stop();
	void clear();
	
	void process(vector<event> &events);
	
	void setBPM(int bpm);
	
	bool getIsPlaying();
	bool getIsRecording();
	
	float getPlayhead();
	float getProgress();
	
	
private:
	
	int sampleRate;
	int blockLength;
	
	int blockIndex;
	int startBlock;
	
	int startTick;
	
	ofxMidi track;
	
	
	bool bPlaying;
	bool bLoaded;
	bool bRecording;
	
	int bpm;
	float ticksPerBlock;
	int nextBPM;
	bool bChangeBPM;
	
	
	
};