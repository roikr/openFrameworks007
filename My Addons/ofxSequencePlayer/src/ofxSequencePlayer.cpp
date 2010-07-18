/*
 *  ofxSequencePlayer.cpp
 *  atlasCreatorFromImages
 *
 *  Created by Roee Kremer on 3/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSequencePlayer.h"
#include "ofMain.h"
#include "ofxDirList.h"


ofxSequencePlayer::ofxSequencePlayer () {
	bLoaded 					= false;
	width 						= 0;
	height						= 0;
	//speed 						= 1;
	//bUseTexture					= true;
	//bStarted					= false;
	//pixels						= NULL;
	nFrames						= 0;
	//bPaused						= false;
	bPlaying					= false;
}

ofxSequencePlayer::~ofxSequencePlayer () {
	
}

bool ofxSequencePlayer::loadMovie(string path,float fps) {
	
	if (!frames.empty()) {
		for (vector<ofImage*>::iterator iter = frames.begin(); iter!=frames.end(); iter++) {
			delete (*iter);
		}
		frames.clear();
	}
	ofxDirList DIR;
	DIR.setVerbose(false);
	DIR.allowExt("png");
	vector<string> comps = ofSplitString(path, "/");
	this->name = comps.back();
    nFrames = DIR.listDir(path+"/.");
	for (int i=0; i<nFrames; i++) {
		ofImage *frame = new ofImage;
		char numStr[3];
		sprintf(numStr, "%02u",i+1);
		
		frame->loadImage(path+"/"+this->name+"_"+numStr+".png");
		frames.push_back(frame);
		
	}
	
	width = frames.front()->getWidth();
	height = frames.front()->getHeight();
	currentFrame = frames.begin();
	bLoaded = true;
	lastFrame = 0;
	this->fps = fps;
}

void ofxSequencePlayer::idleMovie() {		// rename to updateMovie?
	if (bPlaying) {
		
		int current = startFrame + (float)(ofGetElapsedTimeMillis() - startTime) / (1000.0 / fps);
		
		if (current!=lastFrame) {
			bIsFrameNew = true;
		}
		//bIsFrameNew =  current != lastFrame;
		if (bIsFrameNew) {
			lastFrame = current;
			currentFrame = frames.begin()+current % nFrames;
		}
		
	}
}

void ofxSequencePlayer::play() {
	bPlaying = true;
	startTime = ofGetElapsedTimeMillis();
	startFrame = 0;
	currentFrame = frames.begin();
	bIsFrameNew = true;
}

bool ofxSequencePlayer::isFrameNew() {
	return bIsFrameNew;
}

unsigned char * ofxSequencePlayer::getPixels() {
	return (*currentFrame)->getPixels();
}

float ofxSequencePlayer::getPosition() {
	return (float)getCurrentFrame() / (float)(nFrames-1);
}

float ofxSequencePlayer::getDuration() {
	return fps*nFrames;
}

bool ofxSequencePlayer::getIsMovieDone() {
	return currentFrame == frames.end()-1;
}


void ofxSequencePlayer::setPosition(float pct) {
}

void ofxSequencePlayer::setLoopState(int state) {
}

void ofxSequencePlayer::setFrame(int frame) { // frame 0 = first frame...
	currentFrame = frames.begin()+frame;
}


void ofxSequencePlayer::draw(float x, float y, float w, float h) {
	(*currentFrame)->draw(x,y,w,h);
	bIsFrameNew = false;
}

void ofxSequencePlayer::draw(float x, float y) {
	(*currentFrame)->draw(x,y);
	bIsFrameNew = false;
}

void ofxSequencePlayer::setAnchorPoint(int x, int y) {			//set the anchor point in pixels
	(*currentFrame)->setAnchorPoint(x,y);
}

void ofxSequencePlayer::resetAnchor() {
	(*currentFrame)->resetAnchor();
}

void ofxSequencePlayer::setPaused(bool bPause) {
	if (bPause) {
		bPlaying = false;
	} else {
		bPlaying = true;
		startTime = ofGetElapsedTimeMillis();
		startFrame = lastFrame;
		bIsFrameNew = true;
	}

	
	
}

int	ofxSequencePlayer::getCurrentFrame() {
	return distance(frames.begin(),currentFrame);
}

int	ofxSequencePlayer::getTotalNumFrames() {
	return nFrames;
}


void ofxSequencePlayer::firstFrame() {
	currentFrame = frames.begin();
}

void ofxSequencePlayer::nextFrame() {
	currentFrame++;
	if (currentFrame == frames.end()) {
		currentFrame = frames.begin();
	}
}

void ofxSequencePlayer::previousFrame() {
	if (currentFrame == frames.begin()) {
		currentFrame = frames.end()-1;
	} else {
		currentFrame--;
	}

}


int ofxSequencePlayer::getHeight() {
	return height;
}

int ofxSequencePlayer::getWidth() {
	return width;
}
