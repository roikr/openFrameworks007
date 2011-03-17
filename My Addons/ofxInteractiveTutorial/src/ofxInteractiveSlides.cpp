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
	
	citer==slides.end();
	
}

void ofxInteractiveSlides::reset() {
	timerStart = ofGetElapsedTimeMillis();
}

void ofxInteractiveSlides::start(int slideNum) {
	vector<slide>::iterator iter = slides.begin()+slideNum;
	
	if (state==SLIDE_IDLE && !iter->done) {
		
		if (!iter->delay || ofGetElapsedTimeMillis()-timerStart>iter->delay) {
			citer = iter;
			state = citer->predelay ? SLIDE_PRE_DELAY : SLIDE_READY;
			bNeedRefresh = true;
		}
		
	}
	
		
	if (state == SLIDE_PRE_DELAY) {
		preTimerStart = ofGetElapsedTimeMillis();
	}
	
	
}

void ofxInteractiveSlides::done(int slideNum) {
	if (state!=SLIDE_DONE) {
		vector<slide>::iterator iter = slides.begin()+slideNum;
		if (citer!=slides.end() && iter==citer) {
			skip();
		} else {
			iter->done = true;
			eval();
		}
	}
}

void ofxInteractiveSlides::skip() {
	assert(state!=SLIDE_DONE);
	
	
	citer->done = true;
	citer = slides.end();
	timerStart = ofGetElapsedTimeMillis();
	state = SLIDE_IDLE;
	bNeedRefresh = true;
	eval();
}

void ofxInteractiveSlides::update() {
	if (state==SLIDE_IDLE || state==SLIDE_DONE) {
		return;
	}
	
	
	switch (state) {
		case SLIDE_PRE_DELAY:
			if (ofGetElapsedTimeMillis()-preTimerStart > citer->predelay) {
				state = SLIDE_READY;
				bNeedRefresh = true;
			}
		default:
			break;
	}
}


void ofxInteractiveSlides::eval() {
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
}

bool ofxInteractiveSlides::getIsDone(int slideNum) {
	
	return slides[slideNum].done;
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
