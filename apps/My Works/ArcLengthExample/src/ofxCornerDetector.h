/*
 *  ofxCornerDetector.h
 *  ArcLengthExample
 *
 *  Created by Roee Kremer on 6/22/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#define MIN_DIST 7
#define MAX_DIST 9 //(MIN_DIST+2)
#define MAX_ALPHA 150

#include "ofMain.h"

struct corner {
	int index;
	float angle;
	bool concave;
};


struct candidate {
	vector<ofPoint>::iterator iter;
	
	
	float alpha;
	float sharpness;
};


class ofxCornerDetector {
	
public:
	void setup();
	void update();
	
	void addPoint(ofPoint &point);
	vector<corner> &getCorners();
	
	
private:
	
	void checkCandidate(vector<ofPoint>::iterator iter);
	bool isCorner(int index);
	
	vector<ofPoint> curve;
	vector<corner> corners;
	vector<candidate> candidates;
	
	
};
