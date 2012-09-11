//
//  ofxEraser.h
//  emptyExample
//
//  Created by Roee Kremer on 9/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxStroke.h"

class ofxEraser {
public:
    void setup(int size);
    void clear();
    void touch(ofVec2f pos,ofFbo &fbo);
private:
    
    ofxStroke stroke;
    int curveSize;
    int size;
    
};