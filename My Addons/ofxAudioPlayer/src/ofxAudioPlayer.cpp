/*
 *  ofxAudioPlayer.cpp
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxAudioPlayer.h"
#include "ofxAudioSample.h"


//#define LOG_AUDIO_PLAYER

#include <iostream>

void ofxAudioPlayer::setup(ofxAudioSample *sample,int bufferSize,int maxInstances) {
	this->bufferSize = bufferSize;
	this->sample = sample;
	this->maxInstances = maxInstances;
	this->instances.reserve(maxInstances);
	
}


void ofxAudioPlayer::play() {
	sampleInstance si;
	si.speed = 1.0f;
	si.left = 1.0f;
	si.right = 1.0f;
	si.retrigger = true;
	
	trigger(si);
}

void ofxAudioPlayer::trigger(sampleInstance si) {
		
	if (si.retrigger && !instances.empty()) {
		instances.back().bStop = true;
#ifdef LOG_AUDIO_PLAYER
		cout << "retrigger, ";
#endif
	} 
	
	si.pos = 0;
	si.bStop = false;
	
	if (instances.size() < maxInstances) {
		instances.insert(instances.begin(),si);
#ifdef LOG_AUDIO_PLAYER
		cout << "trigger: " << instances.size() << endl; // ", blocks: " << sample.getSamplesPerChannel()/blockLength << endl;
#endif		
	} else {
		cout << "ofxAudioPlayer::trigger - exceed maximum instances: " << maxInstances;
	}

	
	
	
}

void ofxAudioPlayer::stop() {
	for (vector<sampleInstance >::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		iter->bStop = true;
	}
}





void ofxAudioPlayer::mixChannel(float * output, int channel, int nChannels) {
	
	
	for (vector<sampleInstance>::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		//sample.mixChannel(output,channel,nChannels,iter->block,iter->volume,iter->bStop);
	
	
		int n = bufferSize;
		if (iter->pos+bufferSize*iter->speed>=sample->numFrames) {
			//n =   (block+1) * blockLength - samplesPerChannel ;
			n = (sample->numFrames - iter->pos)/iter->speed;
		}
		
		//return (tableBuffer+samplesPerChannel*channel+block*blockLength);
		float *buffer = sample->buffer+sample->numFrames*(sample->nChannels == 2 ? channel : 0)+iter->pos;
		float volume = channel ? iter->right : iter->left; 
		
		if (iter->bStop) {
			float step = 1.0/(n-1);
#ifdef LOG_AUDIO_PLAYER
			cout << "ramp" << endl; //<< step << endl;
#endif
			for (int i=0; i<n; i++) {
				output[i*nChannels+channel]+=buffer[(int)(iter->speed*i)]*volume*((n-1-i)*step);
			}
		} else {
			for (int i=0; i<n; i++) {
				output[i*nChannels+channel]+=buffer[(int)(iter->speed*i)]*volume;
			}
		}
	}
}


/*
void ofxAudioPlayer::audioRequested (float * output, int channel,int bufferSize, int nChannels) {
	if (!bIsPlaying ) {
		return;
	} else {
		
		int n = getIsLastBlock() ? (currentBlock+1) * blockLength - samplesPerChannel : blockLength;
		
		
		
		
		float *buffer = channels == 2 ? getCurrentBlock(channel) : getCurrentBlock(0);
		
		for (int i=0; i<n; i++) {
			output[i*nChannels+channel]=buffer[i];
		}
		
		for (int i=n; i<bufferSize; i++) {
			output[i*nChannels+channel]=0;
		}
		
		
	}
	
}

*/

void ofxAudioPlayer::postProcess() {
	vector<sampleInstance>::iterator iter=instances.begin();
	while (iter!=instances.end()) {
		if (iter->pos+bufferSize*iter->speed>=sample->numFrames || iter->bStop) {
			iter = instances.erase(iter);
#ifdef LOG_AUDIO_PLAYER
			cout << "done" << endl; //<< step << endl;
#endif
		} else {
			iter->pos+=bufferSize*iter->speed;
			iter++;
		}
	}
}

int ofxAudioPlayer::getNumPlaying() {
	return instances.size();
}


	


	


void ofxAudioPlayer::exit() {
	
}






	

