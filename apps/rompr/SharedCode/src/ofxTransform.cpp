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

void ofxTransform::start(int ease_function,ofVec2f pos1,ofVec2f pos2,float scl1,float scl2,float rot1,float rot2) {
    penner.start(POSITION_X, ease_function, pos1.x, pos2.x);
    penner.start(POSITION_Y, ease_function, pos1.y, pos2.y);
    penner.start(SCALE, ease_function, scl1, scl2);
    penner.start(ROTATION, ease_function, rot1, rot2);
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