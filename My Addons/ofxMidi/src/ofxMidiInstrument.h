/*
 *  ofxMidiPlayer.h
 *  midiPlayerExample
 *
 *  Created by Roee Kremer on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;


#include "ofxMidi.h"



class ofxMidiSample;

struct note {
	int midi;
	int velocity;
	ofxMidiSample *sample;
};

class ofxMidiInstrument {
	
	
public:
	
	void setup(int blockLength,int sampleTriggers); // call before loading samples loaded 
	
	void loadSample(string filename,int midi,bool bChokeGroup = false);
	void noteOn(int midi,int velocity);
	void noteOff(int midi);
	void noteOffAll();
	
	void preProcess();
	void mixWithBlocks(float *left,float *right);
	void postProcess(); // to call after each step to process next
	
	void exit();
	
	bool getIsPlaying();
	
	
private:
	map<int,ofxMidiSample*> samples;
	set<int> chokeGroup;
	vector<note> start;
	vector<note> playing;
	vector<int> stop;
	
		
	int blockLength;
	
	
	bool bNoteOffAll;
	int sampleTriggers;
		
};
	/*
	
	void loadSong(string filename);
	int loadLoop(string filename);
	void sendNoteOn(int midiChannel,int midiNote,int velocity);
	void sendNoteOff(int midiChannel,int midiNote,int velocity);
	void startLoop(int loopID);
	void switchLoop(int src,int dest);
	void stopLoop(int LoopID);
	 */
	

	


