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


void ofxInteractiveTutorial::setup() {
	state = TUTORIAL_IDLE;
}

void ofxInteractiveTutorial::addMessage(string& str) {
	messages.push_back(str);
}

void ofxInteractiveTutorial::loadFile(string filename) {
	this->filename = filename;
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile(filename);
	assert(bLoaded);
	
	delay = xml.getAttribute("tutorial", "delay", 3000);
	timesCompleted = xml.getAttribute("tutorial", "timesCompleted", 0);
	
	xml.pushTag("tutorial");
	
	for (int i=0; i<xml.getNumTags("message"); i++) {
		messages.push_back(xml.getValue("message", "", i));
	}
	
	xml.popTag();
	
	citer = messages.begin();

}

void ofxInteractiveTutorial::start() {
	if (messages.size()) {
		timerStart = ofGetElapsedTimeMillis();
		state = TUTORIAL_TIMER_STARTED;
	}
	
	
}

void ofxInteractiveTutorial::update() {
	if (/*citer!=messages.end() &&*/ state == TUTORIAL_TIMER_STARTED && ofGetElapsedTimeMillis()-timerStart > delay) {
		state = TUTORIAL_READY;
		//cout << *citer << endl;
	}
}



string ofxInteractiveTutorial::getCurrentText() {
	return *citer;
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


void ofxInteractiveTutorial::done(int num) {
	if (state==TUTORIAL_DONE || num>=messages.size() ) {
		return;
	}
	
	
	
	if (num == distance(messages.begin(), citer)) {
		citer++;
		state = TUTORIAL_IDLE;
		if (citer==messages.end()) {
			state =  TUTORIAL_DONE;
			timesCompleted++;
			ofxXmlSettings xml;
			bool bLoaded = xml.loadFile(filename);
			assert(bLoaded);
			xml.setAttribute("tutorial", "timesCompleted", timesCompleted,0);
			xml.saveFile(filename);
			
		}
			
		
	}
}

int ofxInteractiveTutorial::getTimesCompleted() {
	return timesCompleted;
}

