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


void ofxInteractiveTutorial::setup(bool bKeepOrder) {
	state = TUTORIAL_IDLE;
	this->bKeepOrder = bKeepOrder;
}

void ofxInteractiveTutorial::addMessage(string& str) {
	messages.push_back(make_pair(str, false));
}

void ofxInteractiveTutorial::loadFile(string filename) {
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile(filename);
	assert(bLoaded);
	
	delay = xml.getAttribute("tutorial", "delay", 3000);
	
	xml.pushTag("tutorial");
	
	for (int i=0; i<xml.getNumTags("message"); i++) {
		messages.push_back(make_pair(xml.getValue("message", "", i), false));
	}
	
	citer = messages.begin();

}

void ofxInteractiveTutorial::start() {
	if (messages.size()) {
		timerStart = ofGetElapsedTimeMillis();
		state = TUTORIAL_TIMER_STARTED;
	}
	
	
}

void ofxInteractiveTutorial::update() {
	if (citer!=messages.end() && state == TUTORIAL_TIMER_STARTED && ofGetElapsedTimeMillis()-timerStart > delay) {
		state = TUTORIAL_TIMER_FINISHED;
		cout << citer->first << endl;
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


void ofxInteractiveTutorial::done(int num) {
	if (num>=messages.size()) {
		return;
	}
	
	if (!bKeepOrder) {
		messages[num].second = true;
	}
	
	
	if (num == distance(messages.begin(), citer)) {
		messages[num].second = true;
		
		while (citer->second && citer!=messages.end()) {
			citer++;
		}
		
		if (citer!=messages.end()) {
			start();
		} else {
			state = TUTORIAL_IDLE;
		}

	}
}

