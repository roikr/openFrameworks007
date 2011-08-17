/*
 *  ofxAudioSampler.cpp
 *  iPhoneDynamicCompressor
 *
 *  Created by Roee Kremer on 12/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxAudioSampler.h"

#include "ofMain.h"
#include <math.h>



	
void ofxAudioSampler::setup(int bufferSize,int numBuffers) {
	this->bufferSize = bufferSize;
	this->numBuffers = numBuffers;
	sample.nChannels = 1;
	sample.numFrames = bufferSize * numBuffers;
	sample.buffer	= new float[sample.numFrames];
	memset(sample.buffer, 0, sample.numFrames * sizeof(float));
}


void ofxAudioSampler::audioReceived( float * input, int bufferSize) {
	if( this->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", this->bufferSize, bufferSize);
		return;
	}	
	
		
	if (getIsRecording()) {
		
		for (int i = 0; i < bufferSize; i++){
			sample.buffer[bufferSize * currentBuffer + i] = input[i];
		}
		currentBuffer++;
		if (currentBuffer>=numBuffers) {
			bRecording = false;
		}
	}
		
}

/*
void ofxAudioSampler::audioRequested( float * output, int bufferSize) {
	if( this->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", this->bufferSize, bufferSize);
		return;
	}	
	
	if (getIsPlaying() ) {
		
		
		if (output) {
			for (int i = 0; i < bufferSize; i++){
				output[i ] = buffer[(int)(pos+speed*i)];
			}
		}
		
		pos+=bufferSize*speed;
		
		if (pos + bufferSize*speed >= numBuffers*bufferSize) {
			stop();
		}
		
	} else {
		if (output) {
			for (int i = 0; i < bufferSize; i++){
				output[i ] = 0;
			}
		}
	}
}



void ofxAudioSampler::mix(float *buffer,int bufferSize,float volume) {
	if( this->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoPlayer: your buffer size was set to %i - but the stream needs a buffer size of %i", this->buffer, bufferSize);
		return;
	}
	
	if (getIsPlaying()) {
		
		for (int i = 0; i < bufferSize; i++) {
			buffer[i ] += this->buffer[(int)(pos+speed*i)]*volume;
		}
		
		pos+=bufferSize*speed;
		
		if (pos + bufferSize*speed >=numBuffers*bufferSize) {
			stop();
		}
	} 
}
*/




void ofxAudioSampler::record() {
	bRecording = true;
	currentBuffer = 0;
	
}

/*
void ofxAudioSampler::play(float speed) {
	state = SAMPLER_PLAYING;
	currentBuffer = 0;
	pos = 0;
	this->speed = speed;
}
*/



void ofxAudioSampler::normalize() {
	double peak = 0;
	for (int i = 0; i < bufferSize * numBuffers; i++){
		peak = fmax(peak,sqrt(sample.buffer[i]*sample.buffer[i]));
	}
	
	for (int i = 0; i < bufferSize * numBuffers; i++){
		sample.buffer[i] /= peak;
	}
}

void ofxAudioSampler::trim(float thresh) {
	
	int i;
	for (i = 0; i < bufferSize * numBuffers; i++){
		if (sqrt(sample.buffer[i]*sample.buffer[i])>thresh) {
			break;
		}
	}
	
	if (i<bufferSize * numBuffers) {
		for (int j = 0; j < bufferSize * numBuffers-i; j++){
			sample.buffer[j] = sample.buffer[j+i];
		}
	}
	
	
}

/*
bool ofxAudioSampler::getIsPlaying() {
	return state == SAMPLER_PLAYING;
}
*/

bool ofxAudioSampler::getIsRecording() {
	return bRecording;
}

float *ofxAudioSampler::getBuffer() {return sample.buffer;}
int ofxAudioSampler::getCurrentBuffer() {return currentBuffer;}
int ofxAudioSampler::getBufferSize() {return bufferSize;}
int ofxAudioSampler::getNumBuffers() {return numBuffers;}
ofxAudioSample* ofxAudioSampler::getAudioSample() {return &sample;}
