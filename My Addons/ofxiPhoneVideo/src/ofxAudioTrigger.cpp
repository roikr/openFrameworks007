/*
 *  ofxAudioTrigger.cpp
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/9/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxAudioTrigger.h"


#include <math.h>
#include <algorithm>

ofxAudioTrigger::ofxAudioTrigger() {
	state=TRIGGER_IDLE;
	amplitude = 0;
	rmsAmplitude = 0;
	peak= 0;
	rmsPeak = 0;
};

void ofxAudioTrigger::setup() {
	
}


void ofxAudioTrigger::update() {
}

void ofxAudioTrigger::draw() {
	
	ofFill();
	
	switch (state) {
		case TRIGGER_IDLE:
			ofSetColor(0,0,255);
			break;
		case TRIGGER_SET:
			ofSetColor(0,255,0);
			break;
		case TRIGGER_TRIGGERED:
			ofSetColor(255,0,0);
			break;
		default:
			break;
	}
	
	
	
	ofRect(0, (1-amplitude)* ofGetHeight(), 10, amplitude*ofGetHeight());
	ofRect(10, (1-rmsAmplitude)* ofGetHeight(), 10, rmsAmplitude*ofGetHeight());
	
	ofSetColor(255, 0, 0);
	ofRect(0, (1-thresh)* ofGetHeight()-1, 10, 3);
	
	
	ofSetColor(0,0,0);
	ofRect(0, (1-peak)* ofGetHeight()-1, 10, 3);
	ofRect(10, (1-rmsPeak)* ofGetHeight()-1, 10, 3);
	
	
	
}

void ofxAudioTrigger::resetTrigger() {
	state = TRIGGER_IDLE;
}

void ofxAudioTrigger::setThresh(float thresh) {
	this->thresh = thresh;
}

void ofxAudioTrigger::setTrigger() {
	state = TRIGGER_SET;
}

int ofxAudioTrigger::getTriggerState() {
	return state;
}


float ofxAudioTrigger::getAmplitude() {return amplitude;};
float ofxAudioTrigger::getRmsAmplitude() {return rmsAmplitude;};

float ofxAudioTrigger::getPeak() {return peak;};
float ofxAudioTrigger::getRmsPeak() {return rmsPeak;};


void ofxAudioTrigger::resetMeters() {
	amplitude=0;
	rmsAmplitude=0;
	peak=0;
	rmsPeak=0;
}



void ofxAudioTrigger::audioReceived( float * input, int bufferSize) {
	
	
	float amp = 0;
	float rms = 0;
	// samples are "interleaved"
	
	for (int i = 0; i < bufferSize; i++){
		float abs = sqrt(input[i]*input[i]);
		amp = max(abs,amp);
		
		//calculate the root mean square amplitude
		rms += abs ;
	}
	
	amplitude = amp;
	rmsAmplitude = rms / bufferSize;
	
	peak = max(peak,amplitude);
	rmsPeak = max(rmsPeak,rmsAmplitude);
	
	if (state == TRIGGER_SET && peak>thresh) {
		state = TRIGGER_TRIGGERED;
	}
	
}



