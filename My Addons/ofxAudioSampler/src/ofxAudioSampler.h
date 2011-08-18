/*
 *  ofxAudioSampler.h
 *  iPhoneDynamicCompressor
 *
 *  Created by Roee Kremer on 12/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once


#include "ofxAudioSample.h"

class ofxAudioSampler {
	
public:
	ofxAudioSampler() : bRecording(false) {};
	void setup(int bufferSize,int numBuffers);
	
	
	void audioReceived( float * input, int bufferSize);
		
	void record();
//	void play(float speed);
	
	void normalize();
	void trim(float thresh);
	
	

	bool getIsRecording();
	
	float *getBuffer();
	int getCurrentBuffer();
	int getBufferSize();
	int getNumBuffers();
	ofxAudioSample* getAudioSample();
	
private:
	
	ofxAudioSample sample;
	
	int bufferSize;
	int numBuffers;
//	int state;
	bool bRecording;
	
	
	int currentBuffer;
	
//	int pos;
//	float speed;
};