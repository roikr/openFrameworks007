/*
 *  ofxMidiSample.cpp
 *  Milgrom
 *
 *  Created by Roee Kremer on 10/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxMidiSample.h"

#include <algorithm> // for find
#include <iostream>



bool ofxMidiSample::loadSample(string filename,int blockLength) {
	this->blockLength = blockLength;
	return sample.load(filename,blockLength);
}


void ofxMidiSample::trigger(int velocity,bool retrigger) {
	if (retrigger && !instances.empty()) {
		instances.back().bStop = true;
		cout << "retrigger, ";
	}
	instance i;
	i.block = 0;
	i.volume = (float)velocity/127.0;
	i.bStop = false;
	instances.push_front(i);
	cout << "trigger: " << instances.size() << endl; // ", blocks: " << sample.getSamplesPerChannel()/blockLength << endl;
	
}

void ofxMidiSample::stop() {
	for (deque<instance >::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		iter->bStop = true;
	}
}

void ofxMidiSample::mixWithBlocks(float *left,float *right) {
	for (deque<instance>::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		sample.mix(left,right,iter->block,iter->volume,iter->bStop);
	}
}

void ofxMidiSample::postProcess() { // to call after processing
	
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

void ofxMidiSample::exit() {
	sample.exit();
}

int ofxMidiSample::getNumPlaying() {
	return instances.size();
}