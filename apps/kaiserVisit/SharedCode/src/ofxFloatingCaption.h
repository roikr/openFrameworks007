//
//  ofxFloatingCaption.h
//  emptyExample
//
//  Created by Roee Kremer on 7/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

class ofxFloatingCaption {
public:
    void setup(ofRectangle rect,float hMargin,float vMargin,float radius);
    void setAnchor(ofVec2f anchor);
    ofVec2f getPos();
    ofVec2f getAnchor();
    
private:
    ofVec2f anchor;
    ofRectangle rect;
    float hMargin;
    float vMargin;
    float radius;
    
    ofVec2f dir;
    ofVec2f pos;
};
