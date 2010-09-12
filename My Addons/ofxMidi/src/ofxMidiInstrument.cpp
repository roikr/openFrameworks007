/*
 *  ofxMidiInstrument.cpp
 *  midiPlayerExample
 *
 *  Created by Roee Kremer on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxMidiInstrument.h"
#include "ofxSndFile.h"
#include <algorithm> // for find
#include <iostream>


void ofxMidiInstrument::loadSample(string filename,int midi,bool bChokeGroup) {
	
	//TODO: add check for existing midi note
	
	ofxSndFile *sndFile = new ofxSndFile();
	if (sndFile->load(filename)) {
		samples[midi] = sndFile;
		
		if (bChokeGroup) {
			//cout << midi << " is in chokeGroup" << endl; // DEBUG
			chokeGroup.insert(midi);
		}
				
		//cout << filename << " loaded as midi note: " << midi << endl;
	}
	else {
		delete sndFile;
	}
}

void ofxMidiInstrument::setup(int blockLength,int sampleRate) {
	
	this->blockLength = blockLength;
	this->sampleRate = sampleRate;
	blockIndex = 0;
	
	for (map<int,ofxSndFile*>::iterator iter=samples.begin() ; iter!=samples.end();iter++)
		iter->second->setup(blockLength);
	
	bNoteOffAll = false;
}


void ofxMidiInstrument::noteOn(int midi,int velocity) {

	note n;
	n.midi = midi;
	n.volume = (float)(velocity)/127;
	map<int,ofxSndFile*>::iterator iter = samples.find(midi);
	
	if (iter!=samples.end()) {
		n.sample = iter->second;
		start.push_back(n);
	}
	

}

void ofxMidiInstrument::noteOff(int midi) {
	stop.push_back(midi);
}

void ofxMidiInstrument::noteOffAll() {
	bNoteOffAll = true;
}



void ofxMidiInstrument::preProcess() {
	
	map<int,ofxSndFile*>::iterator siter;
	vector<note>::iterator piter;
	
	if (bNoteOffAll) {
		bNoteOffAll = false;
		playing.clear();
		stop.clear(); // we don't need to stop because there is nothing to 
	} else {
		for(vector<note>::iterator iter2 = start.begin();iter2!=start.end();iter2++) {
			if (chokeGroup.find(iter2->midi)!=chokeGroup.end()) {
				for (set<int>::iterator iter3 = chokeGroup.begin(); iter3!=chokeGroup.end(); iter3++) {
					if (*iter3!=iter2->midi) {
						noteOff(*iter3);
					}
				}
				break;
			}
			
		}
	
		for(vector<int>::iterator iter1 = stop.begin();iter1!=stop.end();iter1++) {
			
			for (piter = playing.begin(); piter!=playing.end() ; piter++) {
				if (piter->midi==*iter1) {
					playing.erase(piter);
					cout << "note off: " << piter->midi << endl;
					break;
				}
			}
		}
	}
	
	stop.clear();
	
	for(vector<note>::iterator iter2 = start.begin();iter2!=start.end();iter2++) {
		for (piter = playing.begin(); piter!=playing.end() ; piter++) {
			if (piter->midi==iter2->midi) {
				break;
			}
		}
		
		if (piter == playing.end()) { // note is not playing, add it
			playing.push_back(*iter2);
			playing.back().sample->play();
			cout << "note on: " << playing.back().midi << endl;
		} else {                     // retrigger
			piter->volume = iter2->volume;
			piter->sample->play();
			cout << "retrigger: " << piter->midi << endl;
		}

			
	}
	start.clear();
	
}

void ofxMidiInstrument::mixWithBlocks(float *left,float *right,float volume) {
	for (vector<note>::iterator iter=playing.begin() ; iter!=playing.end();iter++) {
		iter->sample->mixWithBlocks(left,right,iter->volume);
	}
}

void ofxMidiInstrument::postProcess() { 
	for (vector<note>::iterator iter=playing.begin() ; iter!=playing.end();iter++) {
		iter->sample->postProcess();
		if (!iter->sample->getIsPlaying()) {
			stop.push_back(iter->midi);
		}
	}
	
	
	vector<note>::iterator piter;
	
	for(vector<int>::iterator iter1 = stop.begin();iter1!=stop.end();iter1++) {
		
		for (piter = playing.begin(); piter!=playing.end() ; piter++) {
			if (piter->midi==*iter1) {
				cout << "finish playing: " << piter->midi << endl;
				playing.erase(piter);
				

				break;
			}
		}
	}
	
	stop.clear();
	
	blockIndex++;
}

bool ofxMidiInstrument::getIsPlaying() {
	return !playing.empty();
}

void ofxMidiInstrument::exit() {
	//TODO: add check to ensure nothing is playing
	for(map<int,ofxSndFile*>::iterator iter = samples.begin();iter!=samples.end();iter++) {
		iter->second->exit();
		delete iter->second;
	}
	samples.clear();
}
