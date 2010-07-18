/*
 *  Player.cpp
 *  yesPlasticServer
 *
 *  Created by Roee Kremer on 2/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Player.h"
#include "ofxXmlSettings.h"
#include <set>
#include <iostream>


void Player::setup(ofxXmlSettings * xml,int soundSet,Mixer *mainMixer,SndThread *mainThread) {
	
	
	enable = xml->getValue("enable", "true",0) == "true";
	
	//setName = xml->getValue("id","");
	setName = xml->getValue("sound_set:id","",soundSet);
	
	cout << "loading set: " << setName << ", number: " << soundSet << endl;
	
	
	

	/*
	 for (i=0;i<xml->getNumTags("hierarchy:id");i++) {
	 string sampleID = xml->getValue("hierarchy:id","",i);
	 hierarchy.push_back(samples[sampleID[0]-'A']);
	 }*/
	
	//scale = xml->getValue("scale",0.0);
	
		
	
	id = xml->getValue("id","");
	touchArea = xml->getValue("touch_area",0);
	
	
	
	set<int> chokeGroup;
	if (multi && xml->tagExists("choke_group")) {
		xml->pushTag("choke_group");
		for (int i=0;i<xml->getNumTags("note");i++) 
			chokeGroup.insert(xml->getValue("note", 0, i));
		xml->popTag();
	}
	
	//xml->pushTag("sound_set",soundSet);
	
	
	
	
	multi = xml->tagExists("multi");
	volume = xml->getValue("volume", 1.0f);
	
	if (xml->tagExists("midi_notes")) {
		xml->pushTag("midi_notes");
		for (int i=0; i<xml->getNumTags("midi_note");i++) {
			int midiNote = (xml->getValue("midi_note", 0, i)-36) % 24;
			midiNotes.push_back(midiNote);
			midiNotesMap[midiNote]=i;
		}
		xml->popTag();
	}
	
	if (multi && xml->tagExists("priorities")) {
		xml->pushTag("priorities");
		for (int i=0;i<xml->getNumTags("priority");i++)
			priorities.push_back(xml->getValue("priority",0,i));
		xml->popTag();
	}
	
	//xml->popTag();
	

		
	string path = ofToDataPath("SOUNDS/"+setName+"/"+setName + "_");
	
	for (int i=0; i<8;i++) {
		
		string soundname = path+ofToString(i+1) + ".wav";
		
		cout << "loading SOUND: " << soundname <<  endl;
		sample smpl;
		
		smpl.audio = new SndWave((char*)soundname.c_str(), READ);
		//smpl.buffer = new SndBuffer(1,smpl.inputf->GetDataFrames()*2,
		cout << smpl.audio->GetDataFrames() << endl;
		smpl.input = new SndIn(smpl.audio,1);
		smpl.input->Disable();
		//smpl.gain = new Gain(-6,smpl.input);
		//mix.AddObj(smpl.gain);
		//mix.AddObj(smpl.input);
		mainMixer->AddObj(smpl.input);
		
		mainThread->AddObj(smpl.audio,SNDIO_IN);
		mainThread->AddObj(smpl.input);
		
		smpl.volume = 1.;
		smpl.bStarted = false;
		
		if (multi)
			smpl.bChokeGroup = chokeGroup.find(i)!=chokeGroup.end();
		else
			smpl.bChokeGroup = false;
		
		samples.push_back(smpl);
	}
	
	//gain.SetInput(&mix);
	//gain.SetGain(-12);
	
	//mainMixer->AddObj(&gain);
	//mainMixer->AddObj(&mix);
	
	
			
	//changeLoop(0);
	
	
	//bChangeVideoSet = false;
	//startThread(true, false);
	
}


/*
void Player::addObjects() {
	
	mainThread->AddObj(smpl.audio,SNDIO_IN);
		
	mainThread->AddObj(smpl.input);
	mainThread->AddObj(smpl.gain);
	mainThread->Insert(smpl.gain,smpl.input);
	mainThread->AddObj(&mix);
	mainThread->Insert(&mix,samples.back().input);
	mainThread->AddObj(&gain);
			
	mainThread->AddObj(smpl.audio,SNDIO_IN);
	
	
	mainThread->AddObj(smpl.input);
	mainThread->AddObj(smpl.gain);
			
	
}
*/

void Player::setMidiTrack(ofxXmlSettings *xml) {
	for (int i=0;i<xml->getNumTags("Event");i++) {
		xml->pushTag("Event",i);
		event e;
		e.time = xml->getValue("Absolute", 0);
		e.note = xml->getAttribute("NoteOn", "Note", 0,0);
		e.velocity = xml->getAttribute("NoteOn", "Velocity", 0, 0);
		xml->popTag();
		song.push_back(e);
	}
	
	songhead = song.begin();
}

void Player::update(int tick){
	if (!enable) 
		return;
	
	
	while (songhead!=song.end() && songhead->time <= tick   ) {
		int note = midiNotesMap[songhead->note];
		//cout << "player: " << id  << ", time: " << songhead->time << ", note: " << note << endl;
		vector<sample>::iterator iter = samples.begin() + note;
		
		iter->bStarted = true;
		iter->audio->SetPos(0.0f);
		iter->input->Enable();
		//iter->gain->SetGainM(((float)songhead->velocity)/127.);
		
		songhead++;
	}
	
	/*
	for (vector<sample>::iterator iter = samples.begin(); iter!=samples.end();iter++) {
		if (iter->bStarted) {
			if (iter->audio->Eof()) {
				iter->bStarted = false;
				iter->input->Disable();
				
			}
		}
		
	}
	 */
	
	
}

bool Player::getIsSongDone() {
	
	if (songhead!=song.end()) 
		return false;
	
	
	for (vector<sample>::iterator iter = samples.begin(); iter!=samples.end();iter++) 
		if (iter->bStarted) 
			return false;
	 
	
	return true;
}

void Player::exit() {
	
	
	for (vector<sample>::iterator iter = samples.begin(); iter!=samples.end();iter++) {
		delete iter->audio;
		delete iter->input;
		//delete iter->gain;
	}
	
	samples.clear();
}



