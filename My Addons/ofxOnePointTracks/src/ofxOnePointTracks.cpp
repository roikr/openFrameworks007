/*
 *  ofxMocha.cpp
 *  mocha2xml
 *
 *  Created by Roee Kremer on 2/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxOnePointTracks.h"
#include "ofxXmlSettings.h"
#include <vector>
#include <iostream>


void ofxOnePointTracks::loadTracks(string filename) {
	ofxXmlSettings xml;
	bool loaded = xml.loadFile(filename);
	//assert(loaded);
	if (loaded) {
		loadTracks(&xml);
	} else {
		cout << "no tracks" << endl;
	}
}


void ofxOnePointTracks::loadTracks(ofxXmlSettings *xml) {
	xml->pushTag("tracks");
	for (int i=0; i<xml->getNumTags("track"); i++) {
		track t;
		t.name=xml->getAttribute("track","name","",i);
		xml->pushTag("track",i);
		for (int j=0; j<xml->getNumTags("point");j++) {
			t.points.push_back(make_pair(xml->getAttribute("point","x",0,j),xml->getAttribute("point","y",0,j)));
		}
		tracks.push_back(t);
		xml->popTag();
	}
}

void ofxOnePointTracks::setCurrentTrack(string trackName) {
	for (currentTrack = tracks.begin(); currentTrack!=tracks.end(); currentTrack++) {
		if (currentTrack->name == trackName) {
			break;
		}
	}
	
	if (currentTrack==tracks.end()) {
		track t;
		t.name = trackName;
		tracks.push_back(t);
		currentTrack = tracks.end()-1;
	}
	
}


bool ofxOnePointTracks::getPoint(int pointNum,int &x,int &y) {
	if (pointNum<currentTrack->points.size()) {
		pair<int,int> point = currentTrack->points[pointNum];
		x = point.first;
		y = point.second;
		//x = currentTrack->points.at(pointNum).first;
//		y = currentTrack->points.at(pointNum).second;
		
		return true;
	}
	return false;
	
}


int ofxOnePointTracks::getNumPoints() {
	return currentTrack->points.size();
}

track &ofxOnePointTracks::getCurrentTrack() {
	return *currentTrack;
}

void dumptrack(track t) {
	cout << "track: " << t.name;
	for (vector<pair<int,int> >::iterator iter = t.points.begin(); iter!=t.points.end(); iter++) {
		cout << ", (" << iter->first << "," << iter->second << ")";
	}
	cout << endl;
}

void ofxOnePointTracks::updatePoint(int pointNum,int x,int y) {
	
	

	if (pointNum >= currentTrack->points.size()) {
		int diff = pointNum-currentTrack->points.size()+1;
		for (int i=0; i<diff; i++) {
			currentTrack->points.push_back(make_pair(x, y));
		}
		
	} else {
		
		currentTrack->points[pointNum] = make_pair(x, y);
	//dumptrack(*currentTrack);
	//dumptrack(tracks[0]);
	}
	
}

void ofxOnePointTracks::saveTracks(string filename) {
	
	ofxXmlSettings xml;
	bool loaded = xml.loadFile(filename);
	
	if (loaded) {
		xml.pushTag("tracks");
		
		for (vector<track>::iterator iter=tracks.begin(); iter!=tracks.end(); iter++) {
			for (int i=0; i<xml.getNumTags("track"); i++) {
				if (xml.getAttribute("track", "name","",  i) == iter->name) {
					xml.removeTag("track", i);
					break;
				}
			}
		}
	} else {
		xml.addTag("tracks");
		xml.pushTag("tracks");
	}
	
	
	for (vector<track>::iterator tracksIter=tracks.begin(); tracksIter!=tracks.end(); tracksIter++) {
		int which = xml.addTag("track");
		
		xml.addAttribute("track", "name", tracksIter->name, which);
		xml.pushTag("track", which);
		
		for (vector<pair<int,int> >::iterator iter=tracksIter->points.begin() ; iter!=tracksIter->points.end(); iter++) {
			which = xml.addTag("point");
			xml.addAttribute("point", "number", distance(tracksIter->points.begin(),iter), which);
			xml.addAttribute("point", "x", iter->first, which);
			xml.addAttribute("point", "y", iter->second, which);
		}
		
		xml.popTag();
	}
	
	xml.popTag();
	
	xml.saveFile(filename);
	
}






