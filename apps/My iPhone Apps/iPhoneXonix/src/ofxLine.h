/*
 *  ofxLine.h
 *  iPhoneTextureExample
 *
 *  Created by Roee Kremer on 3/8/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

class ofxLine {
public:
	
	ofxLine(ofPoint &p1,ofPoint &p2);	
	bool getInterception(ofxLine &l,ofPoint &p);
	void getCrossBorderPoints(ofPoint &p1,ofPoint &p2);
	void draw();
	void dump();
	
private:
	float a;
	float b;
	float c;
	
	ofPoint p1;
	ofPoint p2;
	
	
	
	
};