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


//#define LOG_INTERACTIVE_TUTORIAL


void ofxInteractiveTutorial::loadFile(string filename) {
	this->filename = filename;
	ofxXmlSettings xml;
	bool bLoaded = xml.loadFile(filename);
	assert(bLoaded);
	
	
	timesCompleted = xml.getAttribute("tutorial", "timesCompleted", 0);
	
	xml.pushTag("tutorial");
	
	for (int i=0; i<xml.getNumTags("slide"); i++) {
		tutorialSlide s;
		s.text = xml.getValue("slide", "", i);
		s.predelay = xml.getAttribute("slide", "predelay", 0,i);
		s.delay = xml.getAttribute("slide", "delay", 0,i);
		s.tag = xml.getAttribute("slide", "tag", 0,i);
		slides.push_back(s);
	}
	
	xml.popTag();
	
}

void ofxInteractiveTutorial::start() {
	citer = slides.begin();
	state = citer->predelay ? TUTORIAL_PRE_DELAY : TUTORIAL_READY;
	bNeedRefresh = true;
	
#ifdef LOG_INTERACTIVE_TUTORIAL
	cout << "ofxInteractiveTutorial::start - refresh" << endl;
#endif
	
	
	if (state == TUTORIAL_PRE_DELAY) {
		timerStart = ofGetElapsedTimeMillis();
	}
}


void ofxInteractiveTutorial::skip() {
	if (state==TUTORIAL_IDLE || citer==slides.end() ) {
		return;
	}
	
	
	timerStart = ofGetElapsedTimeMillis();
	state = TUTORIAL_TIMER_STARTED;
	bNeedRefresh = true;
#ifdef LOG_INTERACTIVE_TUTORIAL
	cout << "ofxInteractiveTutorial::skip - refresh" << endl;
#endif
}




void ofxInteractiveTutorial::update() {
	if (citer==slides.end()) {
		return;
	}
	
	
	switch (state) {
		case TUTORIAL_TIMER_STARTED:
			if (ofGetElapsedTimeMillis()-timerStart > citer->delay) {
				bNeedRefresh = true;
#ifdef LOG_INTERACTIVE_TUTORIAL
				cout << "ofxInteractiveTutorial::update - TUTORIAL_TIMER_STARTED - eneded - refresh" << endl;
#endif
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
					state = citer->predelay ? TUTORIAL_PRE_DELAY : TUTORIAL_READY;
					
					if (state == TUTORIAL_PRE_DELAY) {
						timerStart = ofGetElapsedTimeMillis();
					}
				}
				
				//cout << *citer << endl;
			}
			
			break;
		case TUTORIAL_PRE_DELAY:
			if (ofGetElapsedTimeMillis()-timerStart > citer->predelay) {
				state = TUTORIAL_READY;
				bNeedRefresh = true;
#ifdef LOG_INTERACTIVE_TUTORIAL
				cout << "ofxInteractiveTutorial::update - TUTORIAL_PRE_DELAY - ended - refresh" << endl;
#endif
			}
		default:
			break;
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

bool ofxInteractiveTutorial::getIsNeedRefresh() {
	return bNeedRefresh;
}

void ofxInteractiveTutorial::setRefreshed() {
	bNeedRefresh = false;
}

