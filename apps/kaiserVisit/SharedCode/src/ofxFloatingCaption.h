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
    void setup(ofRectangle screenRect,ofRectangle captionRect,float radius,ofVec2f anchor);
    void draw();
    void setAnchor(ofVec2f anchor);
    ofVec2f getPos();
    ofVec2f getAnchor();
    float getFade();
    
private:
    ofVec2f anchor;
    ofRectangle screenRect;
    ofRectangle captionRect;
    
    float radius;
    
    ofVec2f dir;
    ofVec2f pos;
    
    ofVec2f linep1,linep2;
    float fade;
};
