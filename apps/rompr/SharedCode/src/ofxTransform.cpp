//
//  ofxTransform.cpp
//  Rompr
//
//  Created by Roee Kremer on 7/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxTransform.h"

enum {
    POSITION_X,
    POSITION_Y,
    SCALE,
    ROTATION
};

void ofxTransform::setup(ofVec2f pos,float scl,float rot) {
    
    penner.setup(POSITION_X,  pos.x);
    penner.setup(POSITION_Y, pos.y);
    penner.setup(SCALE, scl);
    penner.setup(ROTATION, rot);
}

void ofxTransform::start(int ease_function,ofVec2f pos,float scl,float rot) {
    penner.start(POSITION_X, ease_function, pos.x);
    penner.start(POSITION_Y, ease_function,  pos.y);
    penner.start(SCALE, ease_function, scl);
    penner.start(ROTATION, ease_function, rot);
}

void ofxTransform::update() {
    penner.update();
    pos = ofVec2f(penner.getParam(POSITION_X),penner.getParam(POSITION_Y));
    scl = penner.getParam(SCALE);
    rot = penner.getParam(ROTATION);
}

void ofxTransform::begin() {
    ofPushMatrix();
    ofTranslate(pos);
    ofScale(scl, scl);
    
    ofRotate(rot);
}

void ofxTransform::end() {
    ofPopMatrix();
}

bool ofxTransform::getIsEasing() {
    return penner.getIsEasing(POSITION_X);
}