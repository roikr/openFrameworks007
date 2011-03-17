/*
 *  ofxSimpleSegmentator.h
 *  teeterTotter
 *
 *  Created by Roee Kremer on 3/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofxVectorMath.h"

class ofxSimpleSegmentator {
	
public:
	ofxSimpleSegmentator() : bReady(false) {};
	void load();
	void clear();
	void addVector(ofxVec3f &vec);
	void setROI(ofRectangle &rect);
	void getROI(ofRectangle &rect);
	void save();
	bool getIsReady();
	bool getIsPointInside(ofPoint &pnt);
	bool getIsVectorInside(ofxVec3f &vec);
	void report(stringstream &reportStream);

private:
	
	vector<ofxVec3f> points;
	vector<ofxVec3f> normals;
	ofRectangle roi;
	
	bool bReady;
};