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
#include <vector>


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
		
	void setup(ofxAudioSample *sample,int bufferSize,int maxInstances);
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
	
	vector<sampleInstance > instances; // deque was replace by vector - reservable to avoid reallocations but not optimized for lifo yet
	int maxInstances;
};

 