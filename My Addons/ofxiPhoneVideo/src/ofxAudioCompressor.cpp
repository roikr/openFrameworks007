/*
 *  ofxAudioCompressor.cpp
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxAudioCompressor.h"
#include <math.h>
#include <iostream>
#include <algorithm>

ofxAudioCompressor::ofxAudioCompressor() {
}

void ofxAudioCompressor::setup() {
}

void ofxAudioCompressor::draw() {
}

void ofxAudioCompressor::audioProcess( float * input, float *output,int bufferSize) {
}

void ofxAudioCompressor::audioProcess( float * buffer, int bufferSize) {
	
	int n = 64;
	int m = bufferSize/n;
	
	float maxRms = 0;
	
	for (int j=0; j<m; j++) {
		float rms = 0;
		for (int i = 0; i < n; i++){
			//calculate the root mean square amplitude
			rms += sqrt(buffer[j*m+i]*buffer[j*m+i]);
		}
		rms/=m;
		
		maxRms = std::max(maxRms,rms);
	}
	
	
	for (int i = 0; i < bufferSize; i++){
		buffer[i] *= maxRms * 0.6;
	}
	
	
}
