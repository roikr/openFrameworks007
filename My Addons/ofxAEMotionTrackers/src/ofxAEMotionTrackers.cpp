/*
 *  ofxMocha.cpp
 *  mocha2xml
 *
 *  Created by Roee Kremer on 2/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxAEMotionTrackers.h"
#include "ofxXmlSettings.h"
#include <vector>
#include <iostream>


void ofxAEMotionTrackers::loadTracks(string filename) {
	ofxXmlSettings xml;
	bool loaded = xml.loadFile(filename);
	assert(loaded);
	
	loadTracks(&xml);
	
	//cout << "no tracks" << endl;
	
}


void ofxAEMotionTrackers::loadTracks(ofxXmlSettings *xml) {
	xml->pushTag("movie");
	xml->pushTag("composition");
	xml->pushTag("layer");
	for (int i=0; i< xml->getNumTags("group"); i++) {
		if (xml->getAttribute("group", "name", "", i) == "Motion_Trackers") {
			
			xml->pushTag("group", i);
			xml->pushTag("property");
			for (int j=0; j<xml->getNumTags("property"); j++) {
				xml->pushTag("property",j);
				if (xml->getNumTags("property")) {
					track t;
					t.type=xml->getAttribute("property","type","",j);
					cout << t.type << endl;
					
					for (int k=0; k< xml->getNumTags("property"); k++) {
						if (xml->getAttribute("property", "type", "", k) == "Feature_Center") {
							xml->pushTag("property", k);
							for (int l=0; l<xml->getNumTags("key");l++) {
								string str = xml->getAttribute("key","value","",l);
								vector<string> values = ofSplitString(str, ",");
								int x,y;
								istringstream(values.front()) >>  x;
								istringstream(values.back()) >>  y;
								
								//cout << str << " " << x << " " << y << endl;
								
								t.points.push_back(make_pair(x,y));
							}
							xml->popTag();
						}
					}
					
					tracks.push_back(t);
				}
				xml->popTag();
				
				
			}
			xml->popTag();
			xml->popTag();
			
		}
	}
	xml->popTag();
	xml->popTag();
	xml->popTag();
	
}

bool ofxAEMotionTrackers::getPoint(int trackNum,int pointNum,int &x,int &y) { 
	if (trackNum<tracks.size() && pointNum<tracks[trackNum].points.size()) {
		pair<int,int> point = tracks[trackNum].points[pointNum];
		x = point.first;
		y = point.second;
		//x = currentTrack->points.at(pointNum).first;
		//		y = currentTrack->points.at(pointNum).second;
		
		return true;
	}
	return false;
	
}

int ofxAEMotionTrackers::getNumPoints(int trackNum) {
	return trackNum<tracks.size() ? tracks[trackNum].points.size() : 0;
}

int ofxAEMotionTrackers::getNumTracks() {
	return tracks.size();
}





void dumptrack(track t) {
	cout << "track: " << t.type;
	for (vector<pair<int,int> >::iterator iter = t.points.begin(); iter!=t.points.end(); iter++) {
		cout << ", (" << iter->first << "," << iter->second << ")";
	}
	cout << endl;
}







