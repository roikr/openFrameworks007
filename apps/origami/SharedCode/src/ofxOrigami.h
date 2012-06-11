//
//  ofxOrigami.h
//  foldingFirst
//
//  Created by Roee Kremer on 6/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

struct face {
    vector<ofVec2f> vertices;
    ofColor color;
    
};

class ofxOrigami {
public:
    
    void setup();
    void cut(ofVec2f p0,ofVec2f p1);
    void draw();
    
    
    vector<face> faces;
};