//
//  ofxPenner.cpp
//  animatedRompr
//
//  Created by Roee Kremer on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxPenner.h"

#define EASE_DURATION 0.5f

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

void ofxPenner::setup(int ease,values b,values e) {
    this->ease = ease;
    this->b = this->v = b;
    this->e = e;
    this->c.pos = e.pos-b.pos;
    this->c.scl = e.scl-b.scl;
    this->c.rot = e.rot-b.rot;
    time = ofGetElapsedTimef();
    bEasing = true;
    
}

void ofxPenner::update() {
    if (bEasing) {
        float t = ofGetElapsedTimef();
        float delta = t-time;
        if (delta<EASE_DURATION) {
            switch (ease) {
                case EASE_OUT_QUAD:
                    v.pos.x = easeOutBack(delta, b.pos.x, c.pos.x, EASE_DURATION);
                    v.pos.y = easeOutBack(delta, b.pos.y, c.pos.y, EASE_DURATION);
                    v.scl = easeOutBack(delta, b.scl, c.scl, EASE_DURATION);
                    v.rot = easeOutBack(delta, b.rot, c.rot, EASE_DURATION);
                    break;
                    
                default:
                    break;
            }
            
        } else {
            v = e;
            bEasing = false;
        }
    }
}

void ofxPenner::begin() {
    ofPushMatrix();
    ofTranslate(v.pos);
    ofScale(v.scl, v.scl);
    ofRotate(v.rot);
}

void ofxPenner::end() {
    ofPopMatrix();
}

bool ofxPenner::getIsEasing() {
    return bEasing;
}