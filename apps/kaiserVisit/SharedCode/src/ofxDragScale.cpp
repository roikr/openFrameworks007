//
//  ofxDragScale.cpp
//  dragZoomExample
//
//  Created by Roee Kremer on 8/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxDragScale.h"

enum {
    STATE_IDLE,
    STATE_ANIMATING_DRAG,
    STATE_ANIMATING_SCALE
};

enum {
    SCALE
};

void ofxDragScale::setup(ofRectangle window,float width,float height) {
   
    
    mat.makeIdentityMatrix();
    mat.translate(window.getCenter());
   
    this->window = window;
    this->rect.setFromCenter(ofVec2f(0,0), width, height);
    
    maxZoom = 1.0;
    minZoom = 0.2;
    
    state = STATE_IDLE;
};

void ofxDragScale::draw() {
    
    ofPushStyle();
    ofNoFill();
    ofSetHexColor(0xFF0000);
    ofRect(window);
    
    ofFill();
    ofSetColor(0,0,255,100);
    
    begin();
    ofRect(rect);   
    end();
    
    
//    for (int i = 0; i < MAX_TOUCHES; i++){
//		if (touching[i]) glColor4f(0, 1, 0, 1);
//		else glColor4f(1, 0, 0, 1);
//		float w = 8;
//		ofRect( i * (w + 3), 3, w, w);
//	}
	
	char msg[1000];
	sprintf(msg, " zoom: %.1f \n offset: %.1f, %.1f \n ", mat.getScale().x, mat.getTranslation().x-window.getCenter().x, mat.getTranslation().y-window.getCenter().y);
	glColor4f(0, 0, 0, 1);
	ofDrawBitmapString(msg, 3.0f, 25.0f);
    
    ofPopStyle();
};

void ofxDragScale::begin() {
    ofPushMatrix();
    glMultMatrixf(mat.getPtr());
};

void ofxDragScale::end() {
    ofPopMatrix();
};

//bool ofxDragScale::inside(ofVec2f pos) {
//    
//    ofMatrix4x4 imat = mat.getInverse();
//    return rect.inside(imat.preMult(ofVec3f(pos)));
//}

void ofxDragScale::touchDown(ofTouchEventArgs &touch) {
    ofVec2f pos(touch.x,touch.y);
    if (state!=STATE_ANIMATING_SCALE &&  window.inside(pos)) {
        
//        cout << (inside(pos) ? "" : "not ") << "inside" << endl;
        
        
        touches.push_back(touch);
        
        velocity = ofVec2f(0,0);
        time = ofGetElapsedTimef();
        state = STATE_IDLE;
    }
};

void ofxDragScale::touchMoved(ofTouchEventArgs &touch) {
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
            
            ofVec2f p2(touch.x,touch.y);
            ofVec2f p1(iter->x,iter->y);
            *iter = touch;
            
               
            
            
            switch (touches.size()) {
                case 1: {
                    
                    
                    transform(ofVec2f(0,0),p2-p1,1.0);
                    
                    
                    float newTime = ofGetElapsedTimef();
                    
                    if (newTime-time>0) {
                        velocity = (p2-p1)/(newTime-time)/mat.getScale().x;
                    } else {
                        velocity = ofVec2f(0,0);
                    }
                    
                    
                    time = newTime;
                }    break;
                case 2: {
                    
                    ofTouchEventArgs &second = touches[1-distance(touches.begin(), iter)];
                    ofVec2f q(second.x,second.y);
                    ofVec2f qp1 = q-p1;
                    ofVec2f qp2 = q-p2;
                    
                    ofMatrix4x4 imat = mat.getInverse();
                    ofVec2f anchor = imat.preMult(ofVec3f(0.5*(p1+q)));
                    
                    float scl = qp2.length()/qp1.length();
                    
                    transform(anchor,0.5*(qp1-qp2),scl ); 
                    
                } break;
                default:
                    break;
            }
        }
        
        
    }
};

void ofxDragScale::touchUp(ofTouchEventArgs &touch) {
    if (!touches.empty()) {
        vector<ofTouchEventArgs>::iterator iter;
        for (iter=touches.begin(); iter!=touches.end(); iter++) {
            if (iter->id == touch.id) {
                touches.erase(iter);
                break;
            }
        }
        
        if (touches.empty() && velocity.length()>0) {
            //        cout << velocity.length() << endl;
            state = STATE_ANIMATING_DRAG;
            lastTime = ofGetElapsedTimef();
        } 
    }
};

void ofxDragScale::touchDoubleTap(ofTouchEventArgs &touch) {
    
};

void ofxDragScale::touchCancelled(ofTouchEventArgs &touch) {
    
};



void ofxDragScale::update() {
	
    
	if (state == STATE_ANIMATING_DRAG) {
        if (velocity.length()>0.0001) {
            float t = ofGetElapsedTimef();
            if (transform(ofVec2f(0,0),velocity*(t-lastTime), 1.0)) {
                velocity*=0.8;
                lastTime = t;
            } else {
                state = STATE_IDLE;
            }
        } else {
            state = STATE_IDLE;
        }
		
	}	
    
    if (state == STATE_ANIMATING_SCALE) {
        penner.update();
        ofMatrix4x4 imat = mat.getInverse();
        ofVec2f anchor = imat.preMult(ofVec3f(downPos));
        
        if (!transform(anchor, ofVec2f(0,0), penner.getParam(SCALE)/mat.getScale().x) || !penner.getIsEasing(SCALE)) {
            state = STATE_IDLE;
        } 
        
    }
    
}

bool ofxDragScale::getIsAnimating() {
    return state!=STATE_IDLE;
};


bool ofxDragScale::transform(ofVec2f anchor,ofVec2f trans,float scl) {
    
    ofMatrix4x4 temp(mat);
    
    ofMatrix4x4 scale;
    scale.makeScaleMatrix(scl, scl, 1.0);
    temp.preMult(scale);  
    
    ofVec2f vec = temp.preMult(ofVec3f(anchor))-mat.preMult(ofVec3f(anchor));
//    cout << anchor <<"\t" << vec << "\t" << scl << endl;
    
    temp.translate(trans-vec);
    
//    ofVec3f dim = 0.5*ofVec3f(window.width,window.height)/minZoom;
//    
//    ofVec2f tl = temp.preMult(-dim);
//    ofVec2f br = temp.preMult(dim);
//        
//    if (temp.getScale().x >= minZoom && temp.getScale().x <= maxZoom && tl.x<window.x && tl.y<window.y && br.x>window.x+window.width && br.y>window.y+window.height) {
    
    ofMatrix4x4 imat = temp.getInverse();
    
    if (temp.getScale().x >= minZoom && temp.getScale().x <= maxZoom && rect.inside(imat.preMult(ofVec3f(window.getCenter())))) {
        mat = temp;
        return true;
    }
    
    return false;
}

void ofxDragScale::animateScale(ofVec2f pos,float scale) {
    state = STATE_ANIMATING_SCALE;
    penner.setup(SCALE, mat.getScale().x);
    penner.start(SCALE, EASE_OUT_QUAD, scale);
    downPos = pos;
    
}

