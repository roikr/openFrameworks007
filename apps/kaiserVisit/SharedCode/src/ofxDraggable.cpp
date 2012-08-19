//
//  ofxDraggable.cpp
//  imageLoaderExample
//
//  Created by Roee Kremer on 6/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxDraggable.h"

void ofxDraggable::setup(int width,int height,ofMatrix4x4 mat) {
    
    this->rect.setFromCenter(ofVec2f(0,0), width, height);
    this->mat = mat;
    
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
    glMultMatrixf(mat.getPtr());
};

void ofxDraggable::end() {
    ofPopMatrix();
};

bool ofxDraggable::inside(ofTouchEventArgs &touch) {
    
    return rect.inside(mat.getInverse().preMult(ofVec3f(touch.x,touch.y)));
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
                    mat.postMult(ofMatrix4x4::newTranslationMatrix(ofVec2f(touch.x,touch.y)-ofVec2f(iter->x,iter->y)));
                    

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
                    
                                        
                    ofVec2f trans(0.5*(qp1-qp2));
                    float scl = qp2.length()/qp1.length();
                    float rot = (atan2(qp2.y, qp2.x)-atan2(qp1.y, qp1.x))*180.0f/PI;
                    
                    ofVec2f anchor = mat.getInverse().preMult(ofVec3f(0.5*(p1+q)));
                    
                    ofMatrix4x4 temp(mat);
                    temp.preMult(ofMatrix4x4::newScaleMatrix(scl, scl, 1.0));  
                    temp.preMult(ofMatrix4x4::newRotationMatrix(rot, ofVec3f(0,0,1.0)));
                    
                    ofVec2f vec = temp.preMult(ofVec3f(anchor))-mat.preMult(ofVec3f(anchor));
                    //    cout << anchor <<"\t" << vec << "\t" << scl << endl;
                    
                    temp.postMult(ofMatrix4x4::newTranslationMatrix(trans-vec));
                    mat=temp;
                    
                    
                    
                    
                    
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

