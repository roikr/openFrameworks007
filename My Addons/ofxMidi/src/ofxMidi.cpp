/*
 *  ofxMidi.cpp
 *  midiExample
 *
 *  Created by Roee Kremer on 5/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxMidi.h"
#include "ofxXmlSettings.h"



bool ofxMidi::loadFromXml(string filename) {
	ofxXmlSettings xml;
	
	tracks.clear();
	//events.clear();
	//iter = events.end();
	
	//printf("ofxMidi::loadFromXml: %s\n",filename.c_str());
	
	if (!xml.loadFile(filename)) 
		return false;
	
	xml.pushTag("MIDIFile");
	ticksPerBeat = xml.getValue("TicksPerBeat", 96);
	
	for (int j=0;j<xml.getNumTags("Track");j++) {
	
		xml.pushTag("Track");
		midiTrack t;
		
		for (int i=0; i<xml.getNumTags("Event"); i++) {
			xml.pushTag("Event", i);
			event e;
			e.absolute = xml.getValue("Absolute", 0);
			if (xml.tagExists("NoteOn")) {
				e.channel = xml.getAttribute("NoteOn", "Channel", 1);
				e.note = xml.getAttribute("NoteOn", "Note", 0);
				e.velocity = xml.getAttribute("NoteOn", "Velocity", 64);
				e.bNoteOn = true;
				t.events.push_back(e);
				
			}
			
			if (xml.tagExists("NoteOff")) {
				e.channel = xml.getAttribute("NoteOff", "Channel", 1);
				e.note = xml.getAttribute("NoteOff", "Note", 0);
				e.velocity = xml.getAttribute("NoteOff", "Velocity", 64);
				e.bNoteOn = false;
				t.events.push_back(e);
				
			}
			
			
			xml.popTag();
		}
		
		t.iter = t.events.begin();
		tracks.push_back(t);
		
		xml.popTag(); // Track;
	}
	xml.popTag(); // MIDIFile
	
	return true;
}


void ofxMidi::saveToXml(string filename) {
	ofxXmlSettings xml;
		
	
	//printf("ofxMidi::saveToXml: %s\n",filename.c_str());
	
	xml.addTag("MIDIFile");
	xml.pushTag("MIDIFile");
	
		
		xml.addValue("Format", 0);
		xml.addValue("TrackCount", 1);
		xml.addValue("TicksPerBeat", 96);
		xml.addValue("TimestampType", "Absolute");
		
		for(vector<midiTrack>::iterator trackIter=tracks.begin(); trackIter!=tracks.end();trackIter++) {
			int which = xml.addTag("Track");
			xml.addAttribute("Track", "Number", distance(tracks.begin(), trackIter),which);
			xml.pushTag("Track", which);
			
				for (vector<event>::iterator iter = trackIter->events.begin() ; iter!=trackIter->events.end() ; iter++) {
					int which = xml.addTag("Event");
					xml.pushTag("Event", which);
					xml.addValue("Absolute", iter->absolute);
					string message = iter->bNoteOn ? "NoteOn" : "NoteOff";
					xml.addTag(message);
					xml.addAttribute(message, "Channel", iter->channel,0);
					xml.addAttribute(message, "Note", iter->note,0);
					xml.addAttribute(message, "Velocity", iter->bNoteOn ? iter->velocity : 64,0);
					xml.popTag();
				}
			xml.popTag();
		}
	
	xml.popTag();
	
	xml.saveFile(filename);
	//string test;
//	xml.copyXmlToString(test);
//	cout << test << endl;
	
}



/*
bool ofxMidi::loadFromXml(string filename) {
	ofxXmlSettings xml;
	
	events.clear();
	iter = events.end();
	
	if (!xml.loadFile(filename)) 
		return false;
	
	xml.pushTag("MIDIFile");
	ticksPerBeat = xml.getValue("TicksPerBeat", 96);
	
	xml.pushTag("Track");
	
	for (int i=0; i<xml.getNumTags("Event"); i++) {
		xml.pushTag("Event", i);
		event e;
		e.absolute = xml.getValue("Absolute", 0);
		if (xml.tagExists("NoteOn")) {
			e.channel = xml.getAttribute("NoteOn", "Channel", 1);
			e.note = xml.getAttribute("NoteOn", "Note", 0);
			e.velocity = xml.getAttribute("NoteOn", "Velocity", 64);
			e.bNoteOn = true;
			events.push_back(e);
			
		}
		
		if (xml.tagExists("NoteOff")) {
			e.channel = xml.getAttribute("NoteOff", "Channel", 1);
			e.note = xml.getAttribute("NoteOff", "Note", 0);
			e.velocity = xml.getAttribute("NoteOff", "Velocity", 64);
			e.bNoteOn = false;
			events.push_back(e);
			
		}
		
		
		xml.popTag();
	}
	
	iter = events.begin();
	
	xml.popTag(); // Track;
	xml.popTag(); // MIDIFile
	
	return true;
}
 */

int ofxMidi::getNumTracks() {
	return tracks.size();
}

bool ofxMidi::getCurrentEvent(event &e,int trackNum) {
	if (tracks.at(trackNum).iter!=tracks.at(trackNum).events.end()) {
		e = *(tracks.at(trackNum).iter);
		return true;
	}
	return false;
}

void ofxMidi::nextEvent(int trackNum) {
	if (tracks.at(trackNum).iter!=tracks.at(trackNum).events.end()) {
		(tracks.at(trackNum).iter)++;
	}
}

void ofxMidi::firstEvent(int trackNum) {
	tracks.at(trackNum).iter = tracks.at(trackNum).events.begin();
}

bool ofxMidi::getIsMidiDone(int trackNum) {
	return tracks.at(trackNum).iter==tracks.at(trackNum).events.end();
}

int ofxMidi::getLastTick(int trackNum) {
	if (getNumTracks()) {
		return tracks.at(trackNum).events.back().absolute;
	}
	return 0;
}

int ofxMidi::getTicksPerBeat() {
	return ticksPerBeat;
}

void ofxMidi::clear() {
	tracks.clear();
}

void ofxMidi::addTrack() {
	midiTrack t;
	tracks.push_back(t);
}

void ofxMidi::addEvent(event &e,int trackNum) {
	tracks.at(trackNum).events.push_back(e);
}


