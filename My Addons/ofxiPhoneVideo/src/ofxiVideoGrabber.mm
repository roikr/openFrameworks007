/*
 *  ofxiVideoGrabber.cpp
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiVideoGrabber.h"

#import "MyVideoBuffer.h"
#include <iostream>
#include "ofMain.h"
#include "ofxiPhoneVideo.h"




void ofxiVideoGrabber::setup(ofxiPhoneVideo *video) {
	bIsSessionRunning = false;
	
	this->video = video;
	
	videoTexture = [[[MyVideoBuffer alloc] initWithVideo:video] retain];
	
	buffer				= new float[video->bufferSize];
	memset(buffer, 0, video->bufferSize * sizeof(float));
	
		
	
	bIsRecording = false;
	
	
}

void ofxiVideoGrabber::update() {
	if (bIsRecording) {
		
		if (![videoTexture isRecording]) {
			stopRecording();
		}
	}
	
	while (scaledSamples.size()>=ofGetWidth()) {
		scaledSamples.pop_back();
	}
}

void ofxiVideoGrabber::draw() {
	
	//ofScale(0.5, 0.5, 1);
	//ofScale(1, -1, 1);
	//ofTranslate(0, ofGetHeight());
	//ofRotateZ(-90);
	
	[videoTexture renderCameraToSprite:videoTexture.CameraTexture];
}

void ofxiVideoGrabber::drawAudio() {
	for (deque<float>::iterator iter = scaledSamples.begin(); iter!=scaledSamples.end(); iter++) {
		ofLine(distance(scaledSamples.begin(),iter), ofGetHeight()*(0.5+*iter*0.5), 
			   distance(scaledSamples.begin(),iter), ofGetHeight()*(0.5-*iter*0.5));
	}
}

void ofxiVideoGrabber::exit() {
	stopSession();
	[videoTexture release];
	videoTexture = nil;
	
}


void ofxiVideoGrabber::audioReceived( float * input, int bufferSize) {
	if( video->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoGrabber: your buffer size was set to %i - but the stream needs a buffer size of %i", video->bufferSize, bufferSize);
		return;
	}	
	
	
	float max = 0;
	// samples are "interleaved"
	for (int i = 0; i < bufferSize; i++){
		buffer[i] = input[i];
		max = buffer[i]>max ? buffer[i] : max;
	}
	
	if (bIsRecording) {
		for (int i = 0; i < bufferSize; i++){
			video->sample[video->bufferSize*currentBuffer+i] = input[i];
		}
		currentBuffer++;
		
		if (currentBuffer>=video->numBuffers) {
			stopRecording();
		}
	}
	
	
	scaledSamples.push_front(max);
	
	
}




void ofxiVideoGrabber::startSession() {
	
	if (bIsSessionRunning)
		return;
	
	[videoTexture._session startRunning];
	bIsSessionRunning =  true;
	
}

void ofxiVideoGrabber::stopSession() {
	
	if (!bIsSessionRunning)
		return;
	
	if([videoTexture._session isRunning])
		[videoTexture._session stopRunning];
	
	bIsSessionRunning = false;
}

bool ofxiVideoGrabber::getIsSessionRunning() {
	return bIsSessionRunning;
}


void ofxiVideoGrabber::startRecording() {
	if (bIsRecording) 
		return;
	
	currentBuffer = 0;
	bIsRecording = true;
	[videoTexture record];
	
}

void ofxiVideoGrabber::stopRecording() {
	bIsRecording = false;
}

bool ofxiVideoGrabber::getIsRecording() {
	return bIsRecording;
}







