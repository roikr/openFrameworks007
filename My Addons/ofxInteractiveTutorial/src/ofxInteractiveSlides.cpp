/*
 *  ofxInteractiveSlides.cpp
 *  interactiveTutorialExample
 *
 *  Created by Roee Kremer on 1/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxInteractiveSlides.h"
#include "ofxXmlSettings.h"



void ofxInteractiveSlides::loadFile(string filename) {
	this->filename = filename;
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile(filename);
	assert(bLoaded);
	
	
	timesCompleted = xml.getAttribute("slides", "timesCompleted", 0);
	
	
	xml.pushTag("slides");
	
	for (int i=0; i<xml.getNumTags("slide"); i++) {
		slide s;
		s.text = xml.getValue("slide", "", i);
		s.predelay = xml.getAttribute("slide", "predelay", 0,i);
		s.delay = xml.getAttribute("slide", "delay", 0,i);
		s.tag = xml.getAttribute("slide", "tag", 0,i);
		s.done = false;
		slides.push_back(s);
	}
	
	xml.popTag();
	
	
	
}



void ofxInteractiveSlides::start(int slideNum) {
	if (state==SLIDE_IDLE && !slides[slideNum].done) {
		citer = slides.begin()+slideNum;
		state = citer->predelay ? SLIDE_PRE_DELAY : SLIDE_READY;
		bNeedRefresh = true;
	}
	
		
	if (state == SLIDE_PRE_DELAY) {
		timerStart = ofGetElapsedTimeMillis();
	}
	
	
}

void ofxInteractiveSlides::done(int slideNum) {
	if (state!=SLIDE_DONE) {
		slides[slideNum].done = true;
		state=SLIDE_IDLE;
		citer = slides.begin();
		
		vector<slide>::iterator iter;
		for (iter=slides.begin();iter!=slides.end();iter++ ) {
			if (!iter->done) {
				break;
			}
		}
		
		if (iter==slides.end()) {
			state =  SLIDE_DONE;
			timesCompleted++;
			ofxXmlSettings xml;
			bool bLoaded = xml.loadFile(filename);
			assert(bLoaded);
			xml.setAttribute("slide", "timesCompleted", timesCompleted,0);
			xml.saveFile(filename);
			
		} 
		
		bNeedRefresh = true;
		
	}
}

bool ofxInteractiveSlides::getIsDone(int slideNum) {
	
	return slides[slideNum].done;
}

void ofxInteractiveSlides::skip() {
	if (state==SLIDE_DONE ) {
		return;
	}
	
	
	timerStart = ofGetElapsedTimeMillis();
	state = SLIDE_TIMER_STARTED;
	bNeedRefresh = true;
}




void ofxInteractiveSlides::update() {
	if (state==SLIDE_IDLE || state==SLIDE_DONE) {
		return;
	}
	
	
	switch (state) {
		case SLIDE_TIMER_STARTED: 
			if (ofGetElapsedTimeMillis()-timerStart > citer->delay) {
				state=SLIDE_IDLE;
				citer = slides.end();
				bNeedRefresh = true;
				//cout << *citer << endl;
			}
			
			break;
		case SLIDE_PRE_DELAY:
			if (ofGetElapsedTimeMillis()-timerStart > citer->predelay) {
				state = SLIDE_READY;
				bNeedRefresh = true;
			}
		default:
			break;
	}
	
	
	
}


int	ofxInteractiveSlides::getCurrentSlideNumber() {
	return distance(slides.begin(), citer);
}


int	ofxInteractiveSlides::getCurrentSlideTag() {
	return citer->tag;
}


int ofxInteractiveSlides::getState() {
	return state;
}

void ofxInteractiveSlides::setState(int state) {
	this->state = state;
}

bool ofxInteractiveSlides::getIsNeedRefresh() {
	return bNeedRefresh;
}

void ofxInteractiveSlides::setRefreshed() {
	bNeedRefresh = false;
}
