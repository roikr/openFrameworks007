/*
 *  ofxMidiInstrument.cpp
 *  midiPlayerExample
 *
 *  Created by Roee Kremer on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "ofxMidiInstrument.h"
#include "ofxMidiSample.h"
#include <algorithm> // for find
#include <iostream>


void ofxMidiInstrument::setup(int blockLength,int retriggers) {
	
	this->blockLength = blockLength;
	this->retriggers = retriggers;
	
	/*
	for (map<int,ofxSndFile*>::iterator iter=samples.begin() ; iter!=samples.end();iter++)
		iter->second->setup(blockLength);
	 */
	
	bNoteOffAll = false;
}


void ofxMidiInstrument::loadSample(string filename,int midi,bool bChokeGroup) {
	
	//TODO: add check for existing midi note
	ofxMidiSample *sample = new ofxMidiSample();
		
	if (sample->loadSample(filename,blockLength)) {
		samples[midi] = sample;
		
		if (bChokeGroup) {
			//cout << midi << " is in chokeGroup" << endl; // DEBUG
			chokeGroup.insert(midi);
		}
				
		//cout << filename << " loaded as midi note: " << midi << endl;
	}
	else {
		delete sample;
	}
}



void ofxMidiInstrument::noteOn(int midi,int velocity) {

	note n;
	n.midi = midi;
	n.velocity = velocity;
	map<int,ofxMidiSample*>::iterator iter = samples.find(midi);
	
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
	
	// for notes to start
	vector<note>::iterator siter;
	for(siter = start.begin();siter!=start.end();siter++) {
		vector<note>::iterator piter;
		for (piter = playing.begin(); piter!=playing.end() ; piter++) { // check if allready playing
			if (piter->midi==siter->midi) {
				break;
			}
		}
		
		if (piter == playing.end()) { // note is not playing, add it
			playing.push_back(*siter);
			playing.back().sample->trigger(siter->velocity);
			cout << "note on: " << playing.back().midi << endl;
		} else {                     // retrigger
			if (piter->sample->getNumPlaying() < retriggers) {
				piter->velocity = siter->velocity;
				piter->sample->trigger(piter->velocity);
				cout << "retrigger: " << piter->midi << endl;
			} else {
				cout << "can't retrigger " << piter->midi << ", " << piter->sample->getNumPlaying() << " == " << retriggers<<endl;
			}

			
		}
		
		
	}
	
	
	/*
	// for notes to start
	for(vector<note>::iterator iter2 = start.begin();iter2!=start.end();iter2++) {
		
		for (piter = playing.begin(); piter!=playing.end() ; piter++) { // check if allready playing
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
	 
	 */
	
	if (bNoteOffAll) {
		bNoteOffAll = false;
		
		stop.clear(); // we don't need to stop because there is nothing to 
		for (vector<note>::iterator piter=playing.begin() ; piter!=playing.end();piter++) {
			for (siter = start.begin();siter!=start.end();siter++) {
				if (siter->midi == piter->midi) {
					break;
				}
			}
			if (siter==start.end()) {
				piter->sample->stop();
				stop.push_back(piter->midi);
			}
			
		}
	} 
	else 
	{
		// note off for choke group
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
		// remove note off notes from playing vector - should moved to post
		
	}
	
	start.clear();
	
}

void ofxMidiInstrument::mixWithBlocks(float *left,float *right) {
	for (vector<note>::iterator iter=playing.begin() ; iter!=playing.end();iter++) {
		iter->sample->mixWithBlocks(left,right);
	}
}

void ofxMidiInstrument::postProcess() { 
	
	
	
	for (vector<note>::iterator iter=playing.begin() ; iter!=playing.end();iter++) {
		iter->sample->postProcess();
		if (!iter->sample->getNumPlaying()) {
			stop.push_back(iter->midi);
			cout << "finish playing: " << iter->midi << endl;
		}
	}
	
	for(vector<int>::iterator iter1 = stop.begin();iter1!=stop.end();iter1++) {
		for (vector<note>::iterator piter = playing.begin(); piter!=playing.end() ; piter++) {
			if (piter->midi==*iter1) {
				playing.erase(piter);
				cout << "note off: " << piter->midi << endl;
				break;
			}
		}
	}
	stop.clear();
	
	
}

bool ofxMidiInstrument::getIsPlaying() {
	return !playing.empty();
}

void ofxMidiInstrument::exit() {
	//TODO: add check to ensure nothing is playing
	for(map<int,ofxMidiSample*>::iterator iter = samples.begin();iter!=samples.end();iter++) {
		iter->second->exit();
		delete iter->second;
	}
	samples.clear();
}
