/*
 *  ofxMocha2xml.h
 *  mocha2xml
 *
 *  Created by Roee Kremer on 2/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _OFX_ONE_POINT_TRACKS
#define _OFX_ONE_POINT_TRACKS

#include <string>
#include <vector>
using namespace std;
class ofxXmlSettings;

using namespace std;

struct track {
	vector<pair<int,int> > points;
	string name;
};


class ofxOnePointTracks {
public:
	
	void loadTracks(string filename);
	void loadTracks(ofxXmlSettings *xml);
	void setCurrentTrack(string trackName);
	bool getPoint(int pointNum,int &x,int &y); // pointNum starts from 1
	int getNumPoints();
	track &getCurrentTrack();
	void updatePoint(int pointNum,int x,int y);
	void saveTracks(string filename);
	
	
private:
	vector<track> tracks;
	vector<track>::iterator currentTrack;
};

#endif
