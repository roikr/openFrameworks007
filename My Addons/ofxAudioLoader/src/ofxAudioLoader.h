/*
 *  ofxAudioLoader.h
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 8/22/11.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#pragma once

#include <string>
#include <deque>
#include <AudioToolbox/AudioToolbox.h>
#include "ofxAudioSample.h"

using namespace std;




class ofxAudioLoader {
public:
	
//	ofxAudioLoader();
	
	bool load(string filename);
	
//	void exit();
//	int getSamplesPerChannel();
	
	
//	void setupForSave(int bufferLength); // allocate memory just for current block
//	void openForSave(string filename);
//	void save(float * output,int nChannels);
//	void close();
	
	ofxAudioSample* getAudioSample();
	
private:
	ofxAudioSample audio; // need to clone if using one loader multiple samples
	
	//float* getCurrentBlock(int channel);
	//float* getBlock(int block,int channel);
	
	bool bLoaded;
//	int samplesPerChannel;
//	int bufferLength;
	
//	SInt16 *saveBuffer;
//	AudioBufferList fillBufList;
//	string filename;
	
	
//	ExtAudioFileRef file;


};

