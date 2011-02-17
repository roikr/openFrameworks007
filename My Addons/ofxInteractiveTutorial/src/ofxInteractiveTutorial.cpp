/*
 *  ofxInteractiveTutorial.cpp
 *  interactiveTutorialExample
 *
 *  Created by Roee Kremer on 1/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxInteractiveTutorial.h"
#include "ofxXmlSettings.h"


void ofxInteractiveTutorial::addMessage(string& str,int delay) {
	messages.push_back(make_pair(str,delay));
}

void ofxInteractiveTutorial::loadFile(string filename) {
	this->filename = filename;
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile(filename);
	assert(bLoaded);
	
	
	timesCompleted = xml.getAttribute("tutorial", "timesCompleted", 0);
	
	xml.pushTag("tutorial");
	
	for (int i=0; i<xml.getNumTags("message"); i++) {
		messages.push_back(make_pair(xml.getValue("message", "", i),xml.getAttribute("message", "delay", 0,i)));
	}
	
	xml.popTag();
	
}

void ofxInteractiveTutorial::start() {
	citer = messages.begin();
	state = TUTORIAL_READY;
}


void ofxInteractiveTutorial::skip() {
	if (state==TUTORIAL_DONE || citer==messages.end() ) {
		return;
	}
	
	
	timerStart = ofGetElapsedTimeMillis();
	state = TUTORIAL_TIMER_STARTED;
	
}


void ofxInteractiveTutorial::next() {
	citer++;
	
	if (citer==messages.end()) {
		state =  TUTORIAL_DONE;
		timesCompleted++;
		ofxXmlSettings xml;
		bool bLoaded = xml.loadFile(filename);
		assert(bLoaded);
		xml.setAttribute("tutorial", "timesCompleted", timesCompleted,0);
		xml.saveFile(filename);
		
	} else {
		state = TUTORIAL_READY;
	}
}

void ofxInteractiveTutorial::update() {
	if (citer!=messages.end() && state == TUTORIAL_TIMER_STARTED && ofGetElapsedTimeMillis()-timerStart > citer->second) {
		
		next();
		
		//cout << *citer << endl;
	}
}



string ofxInteractiveTutorial::getCurrentText() {
	return citer->first;
}

int	ofxInteractiveTutorial::getCurrentNumber() {
	return distance(messages.begin(), citer);
}


int ofxInteractiveTutorial::getState() {
	return state;
}

void ofxInteractiveTutorial::setState(int state) {
	this->state = state;
}



int ofxInteractiveTutorial::getTimesCompleted() {
	return timesCompleted;
}

