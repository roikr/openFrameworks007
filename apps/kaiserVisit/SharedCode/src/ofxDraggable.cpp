//
//  ofxDraggable.cpp
//  imageLoaderExample
//
//  Created by Roee Kremer on 6/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxDraggable.h"

void ofxDraggable::setup(ofRectangle rect) {
    trans = ofVec2f(rect.x+rect.width/2,rect.y+rect.height/2);
    scale = 1;
    rotation = 0;
    this->rect = ofRectangle(-rect.width/2,-rect.height/2,rect.width,rect.height);
    
};

void ofxDraggable::draw() {
    ofPushStyle();
    ofSetHexColor(0xFF0000);
    ofNoFill();
    ofRect(ofVec2f(0,0), rect.width, rect.height);
    ofPopStyle();
};

void ofxDraggable::begin() {
    ofPushMatrix();
    ofTranslate(trans);
    ofScale(scale, scale);
    ofRotate(rotation);
    ofTranslate(-rect.width/2, -rect.height/2);
};

void ofxDraggable::end() {
    ofPopMatrix();
};

bool ofxDraggable::inside(ofTouchEventArgs &touch) {
    ofVec2f pos = ((ofVec2f(touch.x,touch.y)-trans)/scale).rotated(-rotation);
    
    return rect.inside(pos);
}

void ofxDraggable::touchDown(ofTouchEventArgs &touch) {
    if (inside(touch)) {
        touches.push_back(touch);
    }
};

void ofxDraggable::touchMoved(ofTouchEventArgs &touch) {
    if (!touches.empty()) {
        vector<ofTouchEventArgs>::iterator iter;
        for (iter=touches.begin(); iter!=touches.end(); iter++) {
            if (iter->id == touch.id) {
                break;
            }
        }
        
        
        if (iter==touches.end()) {
            if (touches.size()<2) {
                touches.push_back(touch);
            }
        } else {
            switch (touches.size()) {
                case 1:
                    trans+=ofVec2f(touch.x,touch.y)-ofVec2f(iter->x,iter->y);
//                    rect.x = trans.x-rect.width/2;
//                    rect.y = trans.y-rect.height/2;
                    *iter = touch;
                    break;
                case 2: {
                    ofVec2f p2(touch.x,touch.y);
                    ofVec2f p1(iter->x,iter->y);
                    *iter = touch;
                    ofTouchEventArgs &second = touches[1-distance(touches.begin(), iter)];
                    ofVec2f q(second.x,second.y);
                    ofVec2f qp1 = q-p1;
                    ofVec2f qp2 = q-p2;
                    float ds = qp2.length()/qp1.length();
                    scale *= ds;
                    float dr = (atan2(qp2.y, qp2.x)-atan2(qp1.y, qp1.x))*180.0f/PI;
                    rotation += dr;
                    ofVec2f tq = trans-q;
                    
                    tq.rotate(dr);
                    tq.scale(ds*tq.length());
                    trans = tq + q;
                    
                } break;
                default:
                    break;
            }
        }
        
        
    }
};

void ofxDraggable::touchUp(ofTouchEventArgs &touch) {
    if (!touches.empty()) {
        vector<ofTouchEventArgs>::iterator iter;
        for (iter=touches.begin(); iter!=touches.end(); iter++) {
            if (iter->id == touch.id) {
                touches.erase(iter);
                break;
            }
        }
    }
};

void ofxDraggable::touchDoubleTap(ofTouchEventArgs &touch) {
    
};

void ofxDraggable::touchCancelled(ofTouchEventArgs &touch) {
    
};

