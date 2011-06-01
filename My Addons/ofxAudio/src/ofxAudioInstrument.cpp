/*
 *  ofxAudioInstrument.cpp
 *  MusicCreed
 *
 *  Created by Roee Kremer on 3/28/11.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "ofxAudioInstrument.h"
#include "ofxAudioSample.h"
#include <algorithm> // for find
#include <iostream>

#define LOG_AUDIO_INSTRUMENT

void ofxAudioInstrument::setup(int blockLength,int sampleTriggers) {
	
	this->blockLength = blockLength;
	this->sampleTriggers = sampleTriggers;
	
	bNoteOffAll = false;
}


void ofxAudioInstrument::loadSample(string filename,int midi,bool bChokeGroup) {
	
	//TODO: add check for existing midi note
	ofxAudioSample *sample = new ofxAudioSample();
		
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



void ofxAudioInstrument::noteOn(int midi,int velocity) {

	note n;
	n.midi = midi;
	n.velocity = velocity;
	map<int,ofxAudioSample*>::iterator iter = samples.find(midi);
	
	if (iter!=samples.end()) {
		n.sample = iter->second;
		start.push_back(n);
	}
	

}

void ofxAudioInstrument::noteOff(int midi) {
	stop.push_back(midi);
	map<int,ofxAudioSample*>::iterator iter = samples.find(midi);
	if (iter!=samples.end()) {
		iter->second->stop();
	}
}

void ofxAudioInstrument::noteOffAll() {
	bNoteOffAll = true;
}



void ofxAudioInstrument::preProcess() {
	
	// for notes to start
	vector<note>::iterator siter;
	for(siter = start.begin();siter!=start.end();siter++) {
		vector<note>::iterator piter;
		for (piter = playing.begin(); piter!=playing.end() ; piter++) { // check if allready playing
			if (piter->midi==siter->midi) {
				break;
			}
		}
		
#ifdef LOG_AUDIO_INSTRUMENT
		cout << "note on: " << siter->midi << endl;
#endif
		
		if (piter == playing.end()) { // note is not playing, add it
			playing.push_back(*siter);
			playing.back().sample->trigger(siter->velocity,false);
		} else {                     // retrigger
			piter->velocity = siter->velocity;
			bool retrigger = piter->sample->getNumPlaying() >= sampleTriggers;
			piter->sample->trigger(piter->velocity,retrigger);
		}
	}
	
	
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

/*
void ofxAudioInstrument::mixWithBlocks(float *left,float *right) {
	for (vector<note>::iterator iter=playing.begin() ; iter!=playing.end();iter++) {
		iter->sample->mixWithBlocks(left,right);
	}
}
*/

void ofxAudioInstrument::mixChannel(float * output, int channel, int nChannels) {
	for (vector<note>::iterator iter=playing.begin() ; iter!=playing.end();iter++) {
		iter->sample->mixChannel(output,channel,nChannels);
	}
}


void ofxAudioInstrument::postProcess() { 
	
	
	
	for (vector<note>::iterator iter=playing.begin() ; iter!=playing.end();iter++) {
		iter->sample->postProcess();
		if (!iter->sample->getNumPlaying()) {
			stop.push_back(iter->midi);
#ifdef LOG_AUDIO_INSTRUMENT
			cout << "finish playing: " << iter->midi << endl;
#endif
		}
	}
	
	for(vector<int>::iterator iter1 = stop.begin();iter1!=stop.end();iter1++) {
		for (vector<note>::iterator piter = playing.begin(); piter!=playing.end() ; piter++) {
			if (piter->midi==*iter1) {
				playing.erase(piter);
#ifdef LOG_AUDIO_INSTRUMENT
				cout << "note off: " << piter->midi << endl;
#endif
				break;
			}
		}
	}
	stop.clear();
	
	
}

bool ofxAudioInstrument::getIsPlaying() {
	return !playing.empty();
}

void ofxAudioInstrument::exit() {
	//TODO: add check to ensure nothing is playing
	for(map<int,ofxAudioSample*>::iterator iter = samples.begin();iter!=samples.end();iter++) {
		iter->second->exit();
		delete iter->second;
	}
	samples.clear();
}
