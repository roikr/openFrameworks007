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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define DEBUG_OFX_MIDI

#ifdef DEBUG_OFX_MIDI
#define ofxMidiLog( s, ... ) \
do { \
printf(s, ##__VA_ARGS__); \
} \
while (0)
#else
#define ofxMidiLog( s, ... ) do {} while (0)
#endif


#define SWAPDWORD(a) ((((a) & 0x000000ff) << 24) | \
(((a) & 0x0000ff00) << 8 ) | \
(((a) & 0x00ff0000) >> 8 ) | \
(((a) & 0xff000000) >> 24))

#define SWAPWORD(a) ((((a) & 0xff00) >> 8) | (((a) & 0xff) << 8))


// Read variable-length integer from stream
int readvar(FILE * f) 
{
	int d;
	d = getc(f);
	if (d & 0x80)
	{
		d &= 0x7f;
		int v;
		do
		{
			v = getc(f);
			d = (d << 7) + (v & 0x7f);
		}
		while (v & 0x80);
	}
	return d;
}

// Read doubleword from stream
int readdword(FILE * f)
{
	int d;
	fread(&d,4,1,f);
	d = SWAPDWORD(d);
	return d;
}

// Read word from stream
int readword(FILE * f)
{
	short int d;
	fread(&d,2,1,f);
	d = SWAPWORD(d);
	return d;
}

// Load chunk header
int loadchunkheader(FILE * f, int &length)
{
	int id;
	id = readdword(f);
	length = readdword(f);
	return id;
}


bool ofxMidi::loadFromMidiFile(string filename) {
	progress = 0;
	tracks.clear();
	
	FILE * f = fopen(filename.c_str(),"rb");
	if (!f) 
		return false;
	int len;
	int id = loadchunkheader(f,len);
	//ofxMidiLog("%08x %d\n",id,len);
	if (id != 'MThd')
	{
		ofxMidiLog("Bad header id\n");
		return false;
	}
	if (len < 6)
	{
		ofxMidiLog("Bad header block length\n");
		return false;
	}
	int format = readword(f);
	//ofxMidiLog("format %d\n", format);
	if (format != 1 && format != 0)
	{
		ofxMidiLog("Unsupported format\n");
		return false;
	}
	int tracks = readword(f);
	int numTracks=tracks;
	//ofxMidiLog("tracks %d\n", tracks);
	ticksPerBeat = readword(f);
	//ofxMidiLog("ppqn %d\n",ppqn); // pulses (clocks) per quater note
	if (ticksPerBeat < 0)
	{
		ofxMidiLog("negative ppqn formats not supported\n");
		return false;
	}
	if (len > 6)
	{
		while (len > 6)
		{
			fgetc(f);
			len--;
		}
	}
	
	int uspertick = (500000 / ticksPerBeat);
	while (!feof(f) && tracks)
	{
		id = loadchunkheader(f,len);
		if (id != 'MTrk')
		{
			ofxMidiLog("Unknown chunk\n");
			return false;
		}
		//ofxMidiLog("\nNew track, length %d\n",len);
		midiTrack t;
		progress = (numTracks-tracks)/numTracks;
		
		int trackend = 0;
		int command = 0;
		int time = 0;
		while (!trackend)
		{
			int dtime = readvar(f);
			time += dtime;
			//ofxMidiLog("%3.3f ",((float)time * (float)uspertick)/1000000.0f);
			int data1 = fgetc(f);
			if (data1 == 0xff)
			{
				data1 = fgetc(f); // sub-command
				len = readvar(f);
				switch (data1)
				{
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
						
						while (len)
						{
							fgetc(f);
							len--;
						}
						
						break;
					case 0x2f:
					{
						trackend = 1;
						ofxMidiLog("Track end\n");
						t.iter = t.events.begin();
						this->tracks.push_back(t);
					}
						break;
					case 0x58: // time signature
					{
						int nn = fgetc(f);
						int dd = fgetc(f);
						int cc = fgetc(f);
						int bb = fgetc(f);
						ofxMidiLog("Time sig: %d:%d, metronome:%d, quarter:%d\n",nn,dd,cc,bb);
//						fgetc(f);
//						fgetc(f);
//						fgetc(f);
//						fgetc(f);
					}
						break;
					case 0x59: // key signature
					{
						int sf = fgetc(f);
						int mi = fgetc(f);
						ofxMidiLog("Key sig: %d %s, %s\n",abs(sf),sf == 0?"c":(sf < 0 ? "flat":"sharp"), mi?"minor":"major");
//						fgetc(f);
//						fgetc(f);
					}
						break;
					case 0x51: // tempo
					{
						int t = 0;
						t = fgetc(f) << 16;
						t |= fgetc(f) << 8;
						t |= fgetc(f);
						ofxMidiLog("Tempo: quarter is %dus (%3.3fs) long - BPM = %3.3f\n",t,t/1000000.0f, 60000000.0f/t);						
						uspertick = t / ticksPerBeat;
					}
						break;
					case 0x21: // obsolete: midi port
					{
//						fgetc(f);
						int pp = fgetc(f);
						ofxMidiLog("[obsolete] midi port: %d\n",pp);
					}
						break;
					case 0x20: // obsolete: midi channel
					{
//						fgetc(f);
						int cc = fgetc(f);
						ofxMidiLog("[obsolete] midi channel: %d\n",cc);
					}
						break;
					case 0x54: // SMPTE offset
					{
						fgetc(f);
						fgetc(f);
						fgetc(f);
						fgetc(f);
						fgetc(f);
//						int hr = fgetc(f);
//						int mn = fgetc(f);
//						int se = fgetc(f);
//						int fr = fgetc(f);
//						int ff = fgetc(f);
						//ofxMidiLog("SMPTE Offset: %dh %dm %ds %dfr %dff\n",hr,mn,se,fr,ff);
					}
						break;
					case 0x7f: // Proprietary event
					{
						ofxMidiLog("Proprietary event ");
						while (len)
						{
//							fgetc(f);
							int d = fgetc(f);
							ofxMidiLog("%02X ",d);
							len--;
						}
						ofxMidiLog("\n");
					}
						break;
					default:
						ofxMidiLog("meta command %02x %d\n", data1, len);
						while (len)
						{
							fgetc(f);
							len--;
						}
				
				}
			}
			else
			{
				if (data1 & 0x80) // new command?
				{
					command = data1;
					data1 = fgetc(f);
				}
				
				switch (command & 0xf0)
				{
					case 0x80: // note off
					case 0x90: // note on
					{
						int data2 = fgetc(f);
						
						event e;
						e.absolute = time;
						
						e.channel = command & 0xf;
						e.note = data1;
						e.velocity = data2;
						e.bNoteOn = (command & 0xf0) == 0x90;
						t.events.push_back(e);
							
//						ofxMidiLog("Note off: channel %d, Oct %d Note %s Velocity %d\n",command & 0xf, (data1/12)-1,note[data1%12], data2);
//						ofxMidiLog("Note %s: channel %d, Oct %d Note %d Velocity %d\n",command & 0xf0 == 0x80 ? "off" : "on",command & 0xf, (data1/12)-1,data1, data2);
					}
						break;
					
					case 0xa0: // Note aftertouch
					{
						fgetc(f);
						//int data2 = fgetc(f);
						//ofxMidiLog("Aftertouch: channel %d, Oct %d, Note %s Aftertouch %d\n",command & 0xf, (data1/12)-1,note[data1%12], data2);
					}
						break;
					case 0xb0: // Controller
					{
						fgetc(f);
						//int data2 = fgetc(f);
						//ofxMidiLog("Controller: channel %d, Controller %s Value %d\n",command & 0xf, controller[data1], data2);
					}
						break;
					case 0xc0: // program change
					{
						//ofxMidiLog("Program change: channel %d, program %d\n",command & 0xf, data1);
					}
						break;
					case 0xd0: // Channel aftertouch
					{
						//ofxMidiLog("Channel aftertouch: channel %d, Aftertouch %d\n",command & 0xf, data1);
					}
						break;
					case 0xe0: // Pitch bend
					{
						fgetc(f);
						//int data2 = fgetc(f);
						//ofxMidiLog("Pitchbend: channel %d, Pitch %d\n",command & 0xf, data1 + (data2 << 7));
					}
						break;
					case 0xf0: // general / immediate
					{
						switch (command)
						{
							case 0xf0: // SysEx
							{
								//ofxMidiLog("SysEx ");
								while (data1 != 0xf7)
								{
									//ofxMidiLog("%02X ", data1);
									data1 = fgetc(f);
								}
								//ofxMidiLog("\n");
								// universal sysexes of note:
								// f0 (05) 7e 7f 09 01 f7 = "general midi enable"
								// f0 (05) 7e 7f 09 00 f7 = "general midi disable"
								// f0 (07) 7f 7f 04 01 ll mm f7 = "master volume", ll mm = 14bit value
								// spec doesn't say that the length byte should be there,
								// but it appears to be (the ones in brackets)
							}
								break;
							case 0xf1: // MTC quater frame
							{
								fgetc(f);
								//int dd = fgetc(f);
								//ofxMidiLog("MTC quater frame %d\n",dd);
							}
								break;
							case 0xf2: // Song position pointer
							{
								fgetc(f);
								fgetc(f);
								//int data1 = fgetc(f);
								//int data2 = fgetc(f);
								//ofxMidiLog("Song position pointer %d\n", data1 + (data2 << 7));
							}
								break;
							case 0xf3: // Song select
							{
								fgetc(f);
								//int song = fgetc(f);
								//ofxMidiLog("Song select %d\n", song);
							}
								break;
							case 0xf6: // Tuning request
								//ofxMidiLog("Tuning request\n");
								break;
							case 0xf8: // MIDI clock
								//ofxMidiLog("MIDI clock\n");
								break;
							case 0xf9: // MIDI Tick
								//ofxMidiLog("MIDI Tick\n");
								break;
							case 0xfa: // MIDI start
								//ofxMidiLog("MIDI start\n");
								break;
							case 0xfc:
								//ofxMidiLog("MIDI stop\n");
								break;
							case 0xfb:
								//ofxMidiLog("MIDI continue\n");
								break;
							case 0xfe:
								//ofxMidiLog("Active sense\n");
								break;
							case 0xff:
								//ofxMidiLog("Reset\n");
								break;
								
							default:
							{
								ofxMidiLog("Unknown: command 0x%02x, data 0x%02x\n", command, data1);
							}
								break;
						}
					}
						break;
					default:
					{
						ofxMidiLog("Unknown: command 0x%02x, data 0x%02x\n", command, data1);
					}
						break;
				}
			}
		}
		
		tracks--;
	}
	return true;
}

bool ofxMidi::loadFromXml(string filename) {
	ofxXmlSettings xml;
	progress = 0;
	tracks.clear();
	//events.clear();
	//iter = events.end();
	
	//ofxMidiLog("ofxMidi::loadFromXml: %s\n",filename.c_str());
	
	if (!xml.loadFile(filename)) 
		return false;
	
	xml.pushTag("MIDIFile");
	ticksPerBeat = xml.getValue("TicksPerBeat", 96);
	
	
	int numTracks = xml.getNumTags("Track");
	for (int j=0;j<numTracks;j++) {
	
		xml.pushTag("Track");
		midiTrack t;
		
		for (int i=0; i<xml.getNumTags("Event"); i++) {
			progress = ((float)j + ((float)i/(float)xml.getNumTags("Event"))) / (float)numTracks;
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

bool ofxMidi::loadMidi(string filename) {
	vector<string> split = ofSplitString(filename, ".");
	if (split.back() == "mid") {
		return loadFromMidiFile(filename);
	} 
	
	if (split.back() == "xml") {
		return loadFromXml(filename);
	} 

	return false;
}

void ofxMidi::saveToXml(string filename) {
	ofxXmlSettings xml;
		
	
	//ofxMidiLog("ofxMidi::saveToXml: %s\n",filename.c_str());
	
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
	if (getNumTracks() && tracks.at(trackNum).events.size() ) {
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

float ofxMidi::getProgress() {
	return progress;
}

void ofxMidi::dumpTracks() {
	for (vector<midiTrack>::iterator iter1=tracks.begin() ; iter1!=tracks.end() ; iter1++) {
		cout << "track:" << endl;
		for (vector<event>::iterator iter2=iter1->events.begin() ; iter2!=iter1->events.end() ; iter2++) {
			cout << "absolute: " << iter2->absolute <<", noteOn: " << iter2->bNoteOn << ", note: " << iter2->note << ", velocity: " << iter2->velocity << endl;
		}
	}
}