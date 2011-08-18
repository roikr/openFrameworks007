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


struct sampleInstance {
	float left;
	float right;
	float speed;
	bool retrigger;
	
	int pos;
	bool bStop;
};


struct ofxAudioSample;

class ofxAudioPlayer {
public:
		
	void setup(ofxAudioSample *sample,int bufferSize);
	void exit();
	
	
	void trigger(sampleInstance si);
	void play();
	void stop();
	
	
	void mixChannel(float * output, int channel, int nChannels);
	void postProcess(); // to call after processing

	int getNumPlaying();
	
private:
	
	int bufferSize;
	
	ofxAudioSample *sample;
	
	deque<sampleInstance > instances;
};

 