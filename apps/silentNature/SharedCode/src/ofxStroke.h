//
//  ofxStroke.h
//  brushStrokesExample
//
//  Created by Roee Kremer on 8/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"


class ofxStroke {
	
public:
	void setup(float spacing);
	void addPatch(vector<ofVec2f> ctrl);
	vector<ofVec2f> &getCurve();
	
    

private:
    vector<ofVec2f> curve;
    float spacing;
};
