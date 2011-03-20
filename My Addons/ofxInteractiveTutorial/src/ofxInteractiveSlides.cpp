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
	
		
	xml.pushTag("slides");
	
	for (int i=0; i<xml.getNumTags("slide"); i++) {
		slide s;
		s.text = xml.getValue("slide", "", i);
		s.predelay = xml.getAttribute("slide", "predelay", 0,i);
		s.delay = xml.getAttribute("slide", "delay", 0,i);
		s.tag = xml.getAttribute("slide", "tag", 0,i);
		s.done = xml.getAttribute("slide", "done", 0,i);
		slides.push_back(s);
	}
	
	xml.popTag();
	
	citer==slides.end();
	eval();
	
}

void ofxInteractiveSlides::reset() {
	timerStart = ofGetElapsedTimeMillis();
	state = SLIDE_IDLE;
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
	if (state!=SLIDE_DONE && !slides[slideNum].done) {
		vector<slide>::iterator iter = slides.begin()+slideNum;
		if (citer!=slides.end() && iter==citer) {
			skip();
		} else {
			iter->done = true;
			eval();
		}
		
		
		ofxXmlSettings xml;
		bool bLoaded = xml.loadFile(filename);
		assert(bLoaded);
		xml.pushTag("slides");
		
		xml.setAttribute("slide", "done", true,slideNum);
		
		xml.popTag();
		xml.saveFile(filename);
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
