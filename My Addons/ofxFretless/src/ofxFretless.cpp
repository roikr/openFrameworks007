/*
 *  ofxFretless.cpp
 *  iPhoneNewerGrabExample
 *
 *  Created by Roee Kremer on 12/7/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxFretless.h"
#include "ofMain.h"



void ofxFretless::record() {
	state = SONG_RECORD;
	startTime = ofGetElapsedTimeMillis();
}

void ofxFretless::addNoteEvent(float x) {
	event e;
	
	e.absolute = ofGetElapsedTimeMillis() - startTime;
	e.note = x;
	
	events.push_back(e);
}

void ofxFretless::stop() {
	state = SONG_IDLE;
}

void ofxFretless::play() {
	startTime = ofGetElapsedTimeMillis();
	iter = events.begin();
	state = SONG_PLAY;
}


void ofxFretless::process(vector<event> &events) {
	int currentTime = ofGetElapsedTimeMillis()  - startTime;
	for (; iter!=this->events.end() && iter->absolute <= currentTime; iter++) {
		events.push_back(*iter);
	}
}

int ofxFretless::getSongState() {
	return state;
}
