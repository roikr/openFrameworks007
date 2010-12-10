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


ofxiVideoGrabber::ofxiVideoGrabber() {
	state = CAMERA_IDLE;
}

int ofxiVideoGrabber::getState() {
	return state;
}
	

void ofxiVideoGrabber::setup(ofxiPhoneVideo *video) {
	
	
	this->video = video;
	
	videoTexture = [[[MyVideoBuffer alloc] initWithVideo:video] retain];
	
	buffer				= new float[video->bufferSize];
	memset(buffer, 0, video->bufferSize * sizeof(float));
	
		
	
	
	
}

void ofxiVideoGrabber::update() {
	
	
//	while (scaledSamples.size()>=ofGetWidth()) {
//		scaledSamples.pop_back();
//	}
}

void ofxiVideoGrabber::draw() {
	
	//ofScale(0.5, 0.5, 1);
	//ofScale(1, -1, 1);
	//ofTranslate(0, ofGetHeight());
	//ofRotateZ(-90);
	
	[videoTexture renderCameraToSprite:videoTexture.CameraTexture];
}



//	for (deque<float>::iterator iter = scaledSamples.begin(); iter!=scaledSamples.end(); iter++) {
//		ofLine(distance(scaledSamples.begin(),iter), ofGetHeight()*(0.5+*iter*0.5), 
//			   distance(scaledSamples.begin(),iter), ofGetHeight()*(0.5-*iter*0.5));
//	}
	

void ofxiVideoGrabber::exit() {
	stopCamera();
	[videoTexture release];
	videoTexture = nil;
	
}


void ofxiVideoGrabber::audioReceived( float * input, int bufferSize) {
	if( video->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoGrabber: your buffer size was set to %i - but the stream needs a buffer size of %i", video->bufferSize, bufferSize);
		return;
	}	
	
	
	
	if (getState()==CAMERA_RECORDING) {
		for (int i = 0; i < bufferSize; i++){
			video->sample[video->bufferSize*currentBuffer+i] = input[i];
		}
		currentBuffer++;
		
		if (currentBuffer>=video->numBuffers) {
			state = CAMERA_RUNNING;
		}
	}
	
	
	//scaledSamples.push_front(max);
	
	
}




void ofxiVideoGrabber::startCamera() {
	
	if (getState()!=CAMERA_IDLE)
		return;
	
	[videoTexture._session startRunning];
	state = CAMERA_RUNNING;
	
}

void ofxiVideoGrabber::stopCamera() {
	
	if (getState()==CAMERA_IDLE)
		return;
	
	if([videoTexture._session isRunning])
		[videoTexture._session stopRunning];
	
	state = CAMERA_IDLE;
}


void ofxiVideoGrabber::startCapture() {
	state = CAMERA_CAPTURING;
	[videoTexture capture];
}

void ofxiVideoGrabber::record() {
	currentBuffer = 0;
	state =CAMERA_RECORDING;
	[videoTexture record];
	
}












