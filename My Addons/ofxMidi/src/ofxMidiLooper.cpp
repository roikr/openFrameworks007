/*
 *  ofxMidiTrack.cpp
 *  midiPlayerExample
 *
 *  Created by Roee Kremer on 7/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxMidiLooper.h"
#include <iostream>


	

void ofxMidiLooper::setup(int blockLength,int sampleRate,int bpm) { // call after all samples loaded 
	
	this->blockLength = blockLength;
	this->sampleRate = sampleRate;
	this->bpm = bpm;
	blockIndex = 0;
	bPlaying = false;
	
	bNewLoop = false;
	bLoopPlaying = false;
	bChangeBPM = false;
	currentLoop = 0;
	
}


void ofxMidiLooper::clear() {
	bPlaying = false; // TODO: a litter sleep after ?
	for (vector<loop>::iterator iter=loops.begin();iter!=loops.end();iter++) {
		delete iter->midiFile; // TODO: destructor ?
	}
	loops.clear();
	
}

int nextPow2(int a){
	// from nehe.gamedev.net lesson 43
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

void ofxMidiLooper::loadLoop(string filename) {
	
	ofxMidi *midiFile = new ofxMidi;
	if (!midiFile->loadFromXml(filename)) {
		delete midiFile;
		return;
	}
	
	int beats = midiFile->getLastTick()/midiFile->getTicksPerBeat();
	int bar = nextPow2(beats)/4;
	
	
	loop l;
	l.midiFile = midiFile;
	l.numTicks = bar * 4 * midiFile->getTicksPerBeat();
	
	//cout << filename << " loaded and has " << l.numTicks << " ticks("<< midiFile->getLastTick() << "), " << beats << " beats and "  << bar << " bars" << endl;
	
	
	loops.push_back(l);
	
	if (loops.size() == 1) {
		float blocksPerMinutes = sampleRate * 60.0 / blockLength;
		ticksPerBlock =  loops.front().midiFile->getTicksPerBeat() * bpm / blocksPerMinutes ;
		currentLoop = 0; // reseting currentLoop
	}
	 
}


void ofxMidiLooper::sync() {
	if (!loops.size())
		return;
	
	startBlock = blockIndex;
	startTick = 0;
	
	
}

void ofxMidiLooper::play() {
	bPlaying = true;
}

void ofxMidiLooper::pause() {
	bPlaying = false;
}

void ofxMidiLooper::setBPM(int bpm) {
	
	bChangeBPM = true;
	nextBPM = bpm;
}


void ofxMidiLooper::playLoop(int i) {
	
	bNewLoop = true;
	nextLoop = i;
	
}

void ofxMidiLooper::stopLoop() {
	bLoopPlaying = false;
}




void ofxMidiLooper::process(vector<event> &events) {
	
	if (bPlaying) {
		
	
		ofxMidi *midiFile;
		
		int currentTick =(int)(startTick+ticksPerBlock * (blockIndex-startBlock)) % loops.at(currentLoop).numTicks;
		
		if (bChangeBPM) {
			//cout << "bpm: " << bpm << " to " << nextBPM << endl;
			
			bChangeBPM = false;
			
			bpm = nextBPM;
			
			startTick = currentTick;
			startBlock = blockIndex;
			float blocksPerMinutes = sampleRate * 60.0 / blockLength;
			ticksPerBlock =  loops.front().midiFile->getTicksPerBeat() * bpm / blocksPerMinutes ;
			//cout << "ticksPerBlock: " << ticksPerBlock << endl;
			
		}
		
		if (bNewLoop) {
			currentLoop = nextLoop;
			bNewLoop = false;
			bLoopPlaying = true;
			
			midiFile = loops.at(currentLoop).midiFile;
			
			//cout << "seekPlayhead - block: " << blockIndex << ", start: " << startBlock << ", currentTick: " << currentTick << endl;
			midiFile->firstEvent();
			
			event e;
			while (midiFile->getCurrentEvent(e) && e.absolute<=currentTick) {
				midiFile->nextEvent();
			}
			
		}
		if (bLoopPlaying) {
		
			midiFile = loops.at(currentLoop).midiFile;
			
			int nextTick = (int)(startTick+ticksPerBlock * (blockIndex-startBlock+1)) % loops.at(currentLoop).numTicks;
			//cout << "process - block: " << blockIndex << ", start: " << startBlock << ", currentTick: " << currentTick << ", nextTick: " << nextTick << endl;
			
			event e;
			
			if (nextTick<currentTick) {
				while (midiFile->getCurrentEvent(e) && e.absolute >= currentTick ) {
					midiFile->nextEvent();
					events.push_back(e);
					//cout << e.absolute << "\t";
				}
				
				midiFile->firstEvent();
			}
			
			while (midiFile->getCurrentEvent(e) && e.absolute < nextTick ) {
				midiFile->nextEvent();
				events.push_back(e);
				//cout << e.absolute << "\t";
			}
			
		}
	}
	blockIndex++;
	
}




