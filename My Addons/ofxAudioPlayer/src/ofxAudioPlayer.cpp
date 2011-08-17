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


//#define LOG_AUDIO_FILE

#include <iostream>

void ofxAudioPlayer::setup(ofxAudioSample *sample,int bufferSize) {
	this->bufferSize = bufferSize;
	this->sample = sample;
}


void ofxAudioPlayer::play() {
	trigger(1.0f, 1.0f, true);
}

void ofxAudioPlayer::trigger(float speed,float volume,bool retrigger) {
	if (retrigger && !instances.empty()) {
		instances.back().bStop = true;
#ifdef LOG_AUDIO_FILE
		cout << "retrigger, ";
#endif
	}
	instance i;
	i.pos = 0;
	i.volume = volume;
	i.bStop = false;
	i.speed = speed;
	instances.push_front(i);
#ifdef LOG_AUDIO_FILE
	cout << "trigger: " << instances.size() << endl; // ", blocks: " << sample.getSamplesPerChannel()/blockLength << endl;
#endif
	
}

void ofxAudioPlayer::stop() {
	for (deque<instance >::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		iter->bStop = true;
	}
}





void ofxAudioPlayer::mixChannel(float * output, int channel, int nChannels) {
	
	
	for (deque<instance>::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		//sample.mixChannel(output,channel,nChannels,iter->block,iter->volume,iter->bStop);
	
	
		int n = bufferSize;
		if (iter->pos+bufferSize*iter->speed>=sample->numFrames) {
			//n =   (block+1) * blockLength - samplesPerChannel ;
			n = (sample->numFrames - iter->pos)/iter->speed;
		}
		
		//return (tableBuffer+samplesPerChannel*channel+block*blockLength);
		float *buffer = sample->buffer+sample->numFrames*(sample->nChannels == 2 ? channel : 0)+iter->pos;
		
		if (iter->bStop) {
			float step = 1.0/(n-1);
#ifdef LOG_AUDIO_FILE
			cout << "ramp" << endl; //<< step << endl;
#endif
			for (int i=0; i<n; i++) {
				output[i*nChannels+channel]+=buffer[(int)(iter->speed*i)]*iter->volume*((n-1-i)*step);
			}
		} else {
			for (int i=0; i<n; i++) {
				output[i*nChannels+channel]+=buffer[(int)(iter->speed*i)]*iter->volume;
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
	deque<instance>::iterator iter=instances.begin();
	while (iter!=instances.end()) {
		if (iter->pos+bufferSize*iter->speed>=sample->numFrames || iter->bStop) {
			iter = instances.erase(iter);
#ifdef LOG_AUDIO_FILE
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






	

