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
	void clear(float spacing);
    void addTouch(ofVec2f pos);
	
	vector<ofVec2f> &getCurve();
	
     deque<ofVec2f> touches;

private:
    void addPatch(vector<ofVec2f> ctrl);
    
    vector<ofVec2f> curve;
   
    float spacing;
};
