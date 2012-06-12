//
//  ofxOrigami.h
//  foldingFirst
//
//  Created by Roee Kremer on 6/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

struct cut {
    cut(int index,ofVec2f p0,ofVec2f p1):index(index),p0(p0),p1(p1) {};
    
    int index;
    ofVec2f p0;
    ofVec2f p1;
   
};

struct face {
    vector<ofVec2f> vertices;
    ofColor color;
    vector<cut> cuts; // cuts appear in the order they had been performed
    
};

class ofxOrigami {
public:
    
    void setup();
    void split(ofVec2f p0,ofVec2f p1);
    void draw();
    
    
    
    list<face> faces; // faces are arranged in a list in the order they are layered
    int numCuts;
    
    void dump(); // for debugging
};