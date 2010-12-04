//
//  ofxiPhoneClipper.m
//  iPhoneGrabExample
//
//  Created by Roee Kremer on 11/8/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "ofxiPhoneClipper.h"
#import "MyBuffer.h"
#include <iostream>
#include "ofMain.h"


enum {
	CLIPPER_IDLE,
	CLIPPER_RECORDING,
	CLIPPER_PLAYING,
	CLIPPER_SCRUBBING
};


void ofxiPhoneClipper::setup() {
	bIsSessionRunning = false;
	
	videoTexture = [[[MyBuffer alloc] init] retain];
	[videoTexture setup:25];
	
	state = CLIPPER_IDLE;
}

void ofxiPhoneClipper::update() {
	if (state == CLIPPER_RECORDING) {
		
		if (![videoTexture isRecording]) {
			play();
		}
	}
	
	if (state == CLIPPER_PLAYING) {
		uint tempFrame = (ofGetElapsedTimeMillis()-start)/40;
		if (tempFrame >= [videoTexture numberOfTextures]) {
			state = CLIPPER_IDLE;
		} else {
			currentTexture = tempFrame;
		}
	}
}

void ofxiPhoneClipper::draw() {
	if (state == CLIPPER_PLAYING || state == CLIPPER_SCRUBBING) {
		[videoTexture renderCameraToSprite:[videoTexture texture:currentTexture]];	
	} else {
		[videoTexture renderCameraToSprite:videoTexture.CameraTexture];
	}
	
	[videoTexture renderSamples];
}



void ofxiPhoneClipper::exit() {
	stopSession();
	[videoTexture release];
	videoTexture = nil;
	
}





void ofxiPhoneClipper::startSession() {
	
	if (bIsSessionRunning)
		return;
	
	[videoTexture._session startRunning];
	bIsSessionRunning =  true;
		
}

void ofxiPhoneClipper::stopSession() {
	
	if (!bIsSessionRunning)
		return;
	
	if([videoTexture._session isRunning])
		[videoTexture._session stopRunning];

	bIsSessionRunning = false;
}

bool ofxiPhoneClipper::getIsSessionRunning() {
	return bIsSessionRunning;
}
		
	
void ofxiPhoneClipper::startRecording() {
	if (state==CLIPPER_RECORDING) 
		return;
	
	state =CLIPPER_RECORDING;
	[videoTexture start];
	
}

void ofxiPhoneClipper::stopRecording() {
	state = CLIPPER_IDLE;
}

bool ofxiPhoneClipper::getIsRecording() {
	return state == CLIPPER_RECORDING;
}


void ofxiPhoneClipper::play() {
	state = CLIPPER_PLAYING;
	start = ofGetElapsedTimeMillis();
	currentTexture = 0;
	
}

void ofxiPhoneClipper::startScrubbing() {
	state = CLIPPER_SCRUBBING;
}

void ofxiPhoneClipper::stopScrubbing() {
	state = CLIPPER_IDLE;
}

bool ofxiPhoneClipper::getIsScrubbing() {
	return state == CLIPPER_SCRUBBING;
}

void ofxiPhoneClipper::setPosition(float pos) {
	currentTexture = pos*([videoTexture numberOfTextures]);
}





