/*
 *  ofxSamplePlayer.cpp
 *  MusicCreed
 *
 *  Created by Roee Kremer on 3/28/11.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSamplePlayer.h"
#include "ofxAudioSample.h"

#include <algorithm> // for find
#include <iostream>

#define LOG_MIDI_SAMPLE


void ofxSamplePlayer::setup(ofxAudioSample *sample,int blockLength) {
	this->blockLength = blockLength;
	this->sample = sample;
}


void ofxSamplePlayer::trigger(int velocity,bool retrigger) {
	if (retrigger && !instances.empty()) {
		instances.back().bStop = true;
#ifdef LOG_MIDI_SAMPLE
		cout << "retrigger, ";
#endif
	}
	instance i;
	i.block = 0;
	i.volume = (float)velocity/127.0;
	i.bStop = false;
	instances.push_front(i);
#ifdef LOG_MIDI_SAMPLE
	cout << "trigger: " << instances.size() << endl; // ", blocks: " << sample.getSamplesPerChannel()/blockLength << endl;
#endif
	
}

void ofxSamplePlayer::stop() {
	for (deque<instance >::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		iter->bStop = true;
	}
}

/*
void ofxSamplePlayer::mixWithBlocks(float *left,float *right) {
	for (deque<instance>::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		sample.mix(left,right,iter->block,iter->volume,iter->bStop);
	}
}
*/

void ofxSamplePlayer::mixChannel(float * output, int channel, int nChannels) {
	for (deque<instance>::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		sample.mixChannel(output,channel,nChannels,iter->block,iter->volume,iter->bStop);
	}
}

void ofxSamplePlayer::postProcess() { // to call after processing
	
	deque<instance>::iterator iter=instances.begin();
	while (iter!=instances.end()) {
		if (sample.getIsLastBlock(iter->block) || iter->bStop) {
			iter = instances.erase(iter);
		} else {
			iter->block++;
			iter++;
		}
	}
	
	
}

void ofxSamplePlayer::exit() {
	sample.exit();
}

int ofxSamplePlayer::getNumPlaying() {
	return instances.size();
}