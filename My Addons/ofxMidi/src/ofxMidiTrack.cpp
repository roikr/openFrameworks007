/*
 *  ofxMidiTrack.cpp
 *  midiPlayerExample
 *
 *  Created by Roee Kremer on 7/21/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxMidiTrack.h"
#include <iostream>


	

void ofxMidiTrack::setup(int blockLength,int sampleRate,int bpm) { // call after all samples loaded 
	
	this->blockLength = blockLength;
	this->sampleRate = sampleRate;
	this->bpm = bpm;
	blockIndex = 0;
	bPlaying = false;
	bRecording = false;
	bLoaded = false;
	
	bChangeBPM = false;
	
	
		
}




void ofxMidiTrack::loadTrack(string filename) {
	
	bLoaded = track.loadMidi(filename);
	if (bLoaded) {
		float blocksPerMinutes = sampleRate * 60.0 / blockLength;
		ticksPerBlock =  track.getTicksPerBeat() * bpm / blocksPerMinutes ;
	}
}

void ofxMidiTrack::saveTrack(string filename) {
	track.saveToXml(filename);
}





void ofxMidiTrack::play() {
	
	startBlock = 0;
	blockIndex = 0;
	startTick = 0;
	track.firstEvent();
	bPlaying = true;
}

void ofxMidiTrack::record() {
	
	startBlock = 0;
	blockIndex = 0;
	startTick = 0;
	track.clear();
	track.addTrack();
	bRecording = true;
}

void ofxMidiTrack::stop() {
	bPlaying = false;
	
	//if (bRecording) {
//		track.saveToXml("test");
//	}
	bRecording = false;
}

void ofxMidiTrack::clear() {
	track.clear();
	track.addTrack();
}


void ofxMidiTrack::setBPM(int bpm) {
	
	bChangeBPM = true;
	nextBPM = bpm;
}




void ofxMidiTrack::process(vector<event> &events) {
	
	if (bPlaying || bRecording) {
		
	

		int currentTick =startTick+ticksPerBlock * (blockIndex-startBlock);
		
		if (bChangeBPM) {
			//cout << "bpm: " << bpm << " to " << nextBPM << endl;
			
			bChangeBPM = false;
			
			bpm = nextBPM;
			
			startTick = currentTick;
			startBlock = blockIndex;
			float blocksPerMinutes = sampleRate * 60.0 / blockLength;
			ticksPerBlock =  track.getTicksPerBeat() * bpm / blocksPerMinutes ;
			//cout << "ticksPerBlock: " << ticksPerBlock << endl;
			
		}
		
		
		if (bPlaying) {
		
	
		
			int nextTick = startTick+ticksPerBlock * (blockIndex-startBlock+1);
			//cout << "process - block: " << blockIndex << ", start: " << startBlock << ", currentTick: " << currentTick << ", nextTick: " << nextTick << endl;
			
			event e;
			
			while (track.getCurrentEvent(e) && e.absolute < nextTick ) {
				track.nextEvent();
				events.push_back(e);
				//cout << e.absolute << "\t";
			}
			if (track.getIsMidiDone()) {
				bPlaying = false;
			}
		}
		
		if (bRecording) {
			for (vector<event>::iterator iter=events.begin(); iter!=events.end(); iter++) {
				iter->absolute = currentTick;
				track.addEvent(*iter);
			}
		}
			
		blockIndex++;
	}
		
	
	
}

bool ofxMidiTrack::getIsPlaying() {
	return bPlaying;
}

bool ofxMidiTrack::getIsRecording() {
	return bRecording;
}

float ofxMidiTrack::getPlayhead() {
	int lastTick = track.getLastTick();
	if (lastTick) {
		event e;
		track.getCurrentEvent(e);
		return (float)e.absolute / (float)lastTick;
	}
	return  0.0f;
	
}

float ofxMidiTrack::getProgress() {
	return track.getProgress();
}
