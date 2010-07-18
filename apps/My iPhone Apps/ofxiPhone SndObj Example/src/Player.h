/*
 *  Player.h
 *  yesPlasticServer
 *
 *  Created by Roee Kremer on 2/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "AudioDefs.h"
#include "ofMain.h"
#include <map>




class ofxXmlSettings;


struct sample {
	SndWave *inputfile;
	SndIn *input;
	Gain *gain;
	bool bChokeGroup;
	float volume;
	bool bStarted;
	
};

struct event{
	int time;
	int note;
	int velocity;
	bool on;
};


class Player {

public:
	
	void setup(ofxXmlSettings * xml,int soundSet,Mixer *main_mixer);
	void update(int tick);
	void exit();
	
	void setMidiTrack(ofxXmlSettings *xml);
	bool getIsSongDone();
	
	string setName;
	bool enable;
	string id;
	int touchArea;
	
	vector<sample> samples;
	vector<int> midiNotes;
	map<int,int> midiNotesMap;
	vector<int> priorities;
	bool multi;
	float volume;
	vector<event> song;
	vector<event>::iterator songhead;
	
	Mixer mix;
	Gain gain;
	
	
	

	
};