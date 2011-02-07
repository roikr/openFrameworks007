/*
 *  ofxCoordinator.h
 *  physicsExample
 *
 *  Created by Roee Kremer on 2/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

class ofxCoordinator {

	public:

	void setup(int width,int height,ofPoint translation,float scale);
	void pushTransform();
	void popTransform();
	ofPoint screenSpaceToWorldSpace(ofPoint pos);

	private:
	
	int width;
	int height;
	ofPoint translation;
	ofPoint scale;
	
};