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


void ofxInteractiveTutorial::addSlide(string& text,int delay,int tag) {
	slide s;
	s.text = text;
	s.delay = delay;
	s.tag = tag;
	slides.push_back(s);
}

void ofxInteractiveTutorial::loadFile(string filename) {
	this->filename = filename;
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile(filename);
	assert(bLoaded);
	
	
	timesCompleted = xml.getAttribute("tutorial", "timesCompleted", 0);
	
	xml.pushTag("tutorial");
	
	for (int i=0; i<xml.getNumTags("slide"); i++) {
		slide s;
		s.text = xml.getValue("slide", "", i);
		s.delay = xml.getAttribute("slide", "delay", 0,i);
		s.tag = xml.getAttribute("slide", "tag", 0,i);
		slides.push_back(s);
	}
	
	xml.popTag();
	
}

void ofxInteractiveTutorial::start() {
	citer = slides.begin();
	state = TUTORIAL_READY;
}


void ofxInteractiveTutorial::skip() {
	if (state==TUTORIAL_IDLE || citer==slides.end() ) {
		return;
	}
	
	
	timerStart = ofGetElapsedTimeMillis();
	state = TUTORIAL_TIMER_STARTED;
	
}


void ofxInteractiveTutorial::next() {
	citer++;
	
	if (citer==slides.end()) {
		state =  TUTORIAL_IDLE;
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
	if (citer!=slides.end() && state == TUTORIAL_TIMER_STARTED && ofGetElapsedTimeMillis()-timerStart > citer->delay) {
		
		next();
		
		//cout << *citer << endl;
	}
}



string ofxInteractiveTutorial::getCurrentSlideText() {
	return citer->text;
}

int	ofxInteractiveTutorial::getCurrentSlideNumber() {
	return distance(slides.begin(), citer);
}

int	ofxInteractiveTutorial::getCurrentSlideTag() {
	return citer->tag;
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

