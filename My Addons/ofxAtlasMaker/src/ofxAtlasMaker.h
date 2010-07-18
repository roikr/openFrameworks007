/*
 *  ofxAtlasMaker.h
 *  lipsAtlasCreator
 *
 *  Created by Roee Kremer on 3/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <vector>
#include <string>

#include "ofxSequencePlayer.h"


using namespace std;

class ofxAtlasMaker {

public:
	
	
	void setup(string name,float &width,float &height,int numFrames);
	void update(int width,int height);
	
	
	void save();
	void process(string path,ofxSequencePlayer &seq,vector<pair<int,int> > *vec = nil);
	
	string getDebugStr();
	
	vector< pair <int,int> > dim;
	vector< pair <int,int> >::iterator iter;

private:
	int width;
	int height;
	int numFrames;
	string name;

	
	
};