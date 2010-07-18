/*
 *  ofxMidi.h
 *  midiExample
 *
 *  Created by Roee Kremer on 5/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>

using namespace std;

struct event {
	int absolute;
	int channel;
	int note;
	int velocity;
	bool bNoteOn;
};

struct midiTrack {
	vector<event> events;
	vector<event>::iterator iter;
};

class ofxMidi {
	
public:
	
	bool loadFromXml(string filename);
	int getNumTracks();
	
	bool getCurrentEvent(event &e,int trackNum=0);
	void nextEvent(int trackNum=0);
	void firstEvent(int trackNum=0);
	bool getIsMidiDone(int trackNum=0);
	int getLastTick(int trackNum=0);
	
	int getTicksPerBeat();
	
	
private:
	vector<midiTrack> tracks;
	
	
	int ticksPerBeat;

};