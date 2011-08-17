/*
 *  ofxAudioFile.h
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#pragma once


#include <string>
#include <deque>


using namespace std;


struct instance {
	int pos;
	float volume;
	float speed;
	bool bStop;
};

struct ofxAudioSample;

class ofxAudioPlayer {
public:
		
	void setup(ofxAudioSample *sample,int bufferSize);
	void exit();
	
	
	void trigger(float speed,float volume,bool retrigger);
	void play();
	void stop();
	
	
	void mixChannel(float * output, int channel, int nChannels);
	void postProcess(); // to call after processing

	int getNumPlaying();
	
private:
	
	int bufferSize;
	
	ofxAudioSample *sample;
	
	deque<instance > instances;
};

 