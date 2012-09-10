//
//  ofxChalk.h
//  emptyExample
//
//  Created by Roee Kremer on 9/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxStroke.h"

class ofxChalk {
public:
    void setup(int width,int height);
    void draw();
    void setColor(ofColor color);
    void clear();
    void touch(ofVec2f pos);
private:
    
    ofImage image;
    ofxStroke stroke;
    int curveSize;
    int step;
    int spacing;
    ofFbo fbo;
    ofFbo strokeFbo;
    ofColor color;
    int size;
    
};