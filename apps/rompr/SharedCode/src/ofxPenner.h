//
//  ofxPenner.h
//  animatedRompr
//
//  Created by Roee Kremer on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

enum {
    EASE_OUT_QUAD,
};

struct values {
    values(ofVec2f pos=ofVec2f(0,0),float scl=0,float rot=0) : pos(pos),scl(scl),rot(rot) {}
    
    ofVec2f pos;
    float scl;
    float rot;
};

class ofxPenner {
public:
    void setup(int ease,values b,values e);
    void update();
    void begin();
    void end();
    bool getIsEasing();
    
private:
    int ease;
    
    values b,e,c,v;
    float time;
    bool bEasing;
    
};
