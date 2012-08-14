//
//  ofxPenner.h
//  animatedRompr
//
//  Created by Roee Kremer on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include <map>

#define EASE_DURATION 0.5f

enum {
    EASE_OUT_BACK,
    EASE_OUT_QUAD
};

struct parameter {
    int ease_function;
    float time;
    float b,e,c,v;
    float duration;
    bool bEasing;
};


class ofxPenner {
public:
    void setup(int paramID,float b);
    void start(int paramID,int ease_function,float e,float duration = EASE_DURATION);
    void update();
    bool getIsEasing(int paramID);
    float getParam(int paramID);
    
private:
    map<int,parameter> params;
    
};
