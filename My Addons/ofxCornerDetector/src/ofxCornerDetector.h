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
	void setup(int min_dist=MIN_DIST,int max_dist=MAX_DIST,int max_alpha=MAX_ALPHA);
	void update();
	
	void addPoint(ofPoint &point);
	
	int getNumCorners();
	int getCornerIndex(int cornerNum);
	float getPathLength(int index1,int index2);
	
	float getTotalPathLength();
	
	
private:
	
	void checkPoint(vector<ofPoint>::iterator iter);
	int firstPass();
	bool areNeighbors(candidate &c1,candidate &c2);
	void addCorner(vector<candidate>::iterator citer);
	int secondPass();
	
	bool isCorner(int index);
	vector<corner> &getCorners();
	
	vector<ofPoint> curve;
	vector<corner> corners;
	vector<candidate> candidates;
	
	int min_dist;
	int max_dist;
	int max_alpha;
	
};
