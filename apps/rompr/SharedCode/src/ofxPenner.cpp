//
//  ofxPenner.cpp
//  animatedRompr
//
//  Created by Roee Kremer on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxPenner.h"


// back easing in - backtracking slightly, then reversing direction and moving to target
// t: current time, b: beginning value, c: c in value, d: duration, s: overshoot amount (optional)
// t and d can be in frames or seconds/milliseconds
// s controls the amount of overshoot: higher s means greater overshoot
// s has a default value of 1.70158, which produces an overshoot of 10 percent
// s==0 produces cubic easing with no overshoot
inline float easeOutBack(float t, float b, float c, float d, float s=1.70158);

inline float easeOutBack(float t, float b, float c, float d, float s) {
    t=t/d-1;
    return c*(t*t*((s+1)*t + s) + 1) + b;
    // c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

inline float easeOutQuad(float t, float b, float c, float d) {
    return -c *(t/=d)*(t-2) + b;
};
 

void ofxPenner::start(int paramID,int ease_function,float b,float e,float duration) {
    parameter p;

    p.ease_function = ease_function;
    p.b = p.v = b;
    p.e = e;
    p.c = p.e-p.b;

    p.time = ofGetElapsedTimef();
    p.bEasing = true;
    p.duration = duration;
    
    params[paramID] = p;
    
    
    
}

void ofxPenner::update() {
    float t = ofGetElapsedTimef();
    for (map<int,parameter>::iterator iter=params.begin(); iter!=params.end(); iter++) {
        parameter &p = iter->second;
        if (p.bEasing) {
       
            float delta = t-p.time;
            if (delta<p.duration) {
                switch (p.ease_function) {
                    case EASE_OUT_QUAD:
                        p.v = easeOutBack(delta, p.b, p.c, p.duration);
                        break;
                        
                    default:
                        break;
                }
                
            } else {
                p.v = p.e;
                p.bEasing = false;
            }
        }
    }
}


bool ofxPenner::getIsEasing(int paramID) {
    map<int,parameter>::iterator iter=params.find(paramID);
    return iter!=params.end() ? iter->second.bEasing : false;
}

float ofxPenner::getParam(int paramID) {
    map<int,parameter>::iterator iter=params.find(paramID);
    return iter!=params.end() ? iter->second.v : 0;
}