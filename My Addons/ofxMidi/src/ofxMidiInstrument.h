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

using namespace std;


#include "ofxMidi.h"


class ofxSndFile;

//enum {
//	SAMPLE_STATUS_START,
//	SAMPLE_STATUS_PLAYING,
//	SAMPLE_STATUS_STOP
//};

struct note {
	int midi;
	float volume;
	ofxSndFile *sample;
};

class ofxMidiInstrument {
	
	
public:
	
	void loadSample(string filename,int midi);
	void setup(int blockLength,int sampleRate=44100); // call after all samples loaded 
	void noteOn(int midi,int velocity);
	void noteOff(int midi);
	
	void preProcess();
	void mixWithBlocks(float *left,float *right,float volume=1.0f);
	void postProcess(); // to call after each step to process next
	
	void exit();
	
	
private:
	map<int,ofxSndFile*> samples;
	vector<note> start;
	vector<note> playing;
	vector<int> stop;
	
	int sampleRate;
	int blockIndex;
	
	int blockLength;
	
	
	
	
	
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
	

	


