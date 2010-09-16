/*
 *  ofxMocha2xml.h
 *  mocha2xml
 *
 *  Created by Roee Kremer on 2/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _OFX_AE_MOTION_TRACKERS
#define _OFX_AE_MOTION_TRACKERS

#include <string>
#include <vector>
using namespace std;
class ofxXmlSettings;

using namespace std;

struct track {
	vector<pair<int,int> > points;
	string type;
};


class ofxAEMotionTrackers {
public:
	
	void loadTracks(string filename);
	void loadTracks(ofxXmlSettings *xml);
	
	bool getPoint(int trackNum,int pointNum,int &x,int &y); // pointNum starts from 1
	int getNumPoints(int trackNum);
	int getNumTracks();
	
private:
	vector<track> tracks;
	
};

#endif
