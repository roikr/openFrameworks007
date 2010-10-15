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

void ofxMidiSample::trigger(int velocity) {
	bStop = false;
	instances.push_front(make_pair(0, (float)velocity/127.0));
	cout << "trigger: " << instances.size() << ", blocks: " << sample.getSamplesPerChannel()/blockLength << endl;
}

void ofxMidiSample::stop() {
	bStop = true;
}

void ofxMidiSample::mixWithBlocks(float *left,float *right) {
	for (deque<pair<int,float> >::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		sample.mix(left,right,iter->first,iter->second,bStop);
	}
}

void ofxMidiSample::postProcess() { // to call after processing
	if (bStop) {
		instances.clear();
	} else {
		deque<pair<int,float> >::iterator iter=instances.begin();
		while (iter!=instances.end() && !sample.getIsLastBlock(iter->first) ) {
			iter->first++;
			iter++;
		}
		if (iter!=instances.end()) {
			instances.erase(iter,instances.end()); // remove all that are not playing !
		}
	}
}

void ofxMidiSample::exit() {
	sample.exit();
}

int ofxMidiSample::getNumPlaying() {
	return instances.size();
}