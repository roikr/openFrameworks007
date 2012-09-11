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
	void clear();
    void touch(ofVec2f pos);
	
	vector<ofVec2f > &getCurve() {return curve;}
    vector<ofVec2f > &getVelocities() {return velocities;}
	
    deque<ofVec2f> touches;
    deque<ofVec2f> touches2;
    

private:
    
    
    vector<ofVec2f > curve;
    vector<ofVec2f > velocities;
//    vector<ofVec2f > diff;
   
   
    float spacing;
   
};
