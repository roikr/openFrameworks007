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
    STATE_ANIMATING_SCALE,
    STATE_ANIMATING_BACK
};

enum {
    PARAM_SCALE,
    PARAM_X,
    PARAM_Y
};

void ofxDragScale::setup(ofRectangle window,float width,float height,ofMatrix4x4 mat,ofMatrix4x4 screenMat) {
    this->mat = this->screenMat = screenMat;
    
    this->mat.preMult(ofMatrix4x4::newTranslationMatrix(window.getCenter()));
    this->xformMat = mat;
    this->mat.preMult(this->xformMat);
  
    //this->mat.preMult(mat);
    imat = this->mat.getInverse();
   
    this->window = window;
    this->rect.setFromCenter(ofVec2f(0,0), width, height);
    
    setMaxZoom(1);
    setMinZoom(0.2);
    
    
    state = STATE_IDLE;
};

void ofxDragScale::debugDraw() {
    
    ofPushStyle();
    ofNoFill();
    ofSetHexColor(0xFF0000);
    ofPushMatrix();
    glMultMatrixf(screenMat.getPtr());
    ofRect(window);
    ofPopMatrix();
    
    ofFill();
    ofSetColor(0,0,255,100);
    
    begin();
    ofRect(rect);   
    end();
    
    ofPopStyle();
}

void ofxDragScale::draw() {
    ofPushStyle();
    
//    for (int i = 0; i < MAX_TOUCHES; i++){
//		if (touching[i]) glColor4f(0, 1, 0, 1);
//		else glColor4f(1, 0, 0, 1);
//		float w = 8;
//		ofRect( i * (w + 3), 3, w, w);
//	}
	
	char msg[1000];
    float scale = mat.getScale().x/screenMat.getScale().x;
    ofVec2f offset((mat.getTranslation()-screenMat.preMult(ofVec3f(window.getCenter())))/screenMat.getScale().x);
	sprintf(msg, " zoom: %.2f \n offset: %.1f, %.1f \n ", scale, offset.x, offset.y);
	glColor4f(1, 0, 0, 1);
	ofDrawBitmapString(msg, 50.0f, 50.0f);
    
    ofPopStyle();
};

void ofxDragScale::begin() {
    ofPushMatrix();
    glMultMatrixf(mat.getPtr());
};

void ofxDragScale::end() {
    ofPopMatrix();
};

bool ofxDragScale::inside(ofVec2f pos) {
    return window.inside(screenMat.getInverse().preMult(ofVec3f(pos)));
   
//    ofMatrix4x4 imat = mat.getInverse();
//    return rect.inside(imat.preMult(ofVec3f(pos)));
}

void ofxDragScale::setMinZoom(float minZoom) { 
    this->minZoom = max(minZoom,min(window.width/rect.width,window.height/rect.height));
    maxZoom = max(minZoom,maxZoom);
    cout << "minZoom: " << this->minZoom << endl;
}

void ofxDragScale::setMaxZoom(float maxZoom) { 
    this->maxZoom = max(minZoom,maxZoom);
    cout << "maxZoom: " << this->maxZoom << endl;
}

void ofxDragScale::touchDown(ofTouchEventArgs &touch) {
    
    
 
        
    if (state!=STATE_ANIMATING_SCALE &&  inside(ofVec2f(touch.x,touch.y))) {
        if (!shouldDecay()) {
            gmat = mat;
            
        }
//        cout << (inside(pos) ? "" : "not ") << "inside" << endl;
        
        
        touches.push_back(touch);
        
        state = STATE_IDLE;
        bMoving = false;
        lastMove = ofGetElapsedTimeMillis();
    }
};

void ofxDragScale::touchMoved(ofTouchEventArgs &touch) {
    if (!touches.empty()) {
        
        if (!shouldDecay()) {
            gmat = mat;
            
        }      
        
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
                    
                    if (!bMoving || ofGetElapsedTimeMillis()-lastMove>500) {
                        bMoving = true;
                        startMove = ofGetElapsedTimeMillis();
                        translation = ofVec2f(0,0);
                    }
                    lastMove = ofGetElapsedTimeMillis();
                    
                    ofVec2f trans(p2-p1);
                    translation+=trans;
                    velocity = translation/(float)(lastMove-startMove)*1000.0;
                   
                    
                    if (shouldDecay()) {
                        trans *= 0.5;
                    }

                    transform(ofVec2f(0,0),trans,1.0);
                    
                   
                }    break;
                case 2: {
                    
                    ofTouchEventArgs &second = touches[1-distance(touches.begin(), iter)];
                    ofVec2f q(second.x,second.y);
                    ofVec2f qp1 = q-p1;
                    ofVec2f qp2 = q-p2;
                    
                    
                    ofVec2f anchor = imat.preMult(ofVec3f(0.5*(p1+q)));
                    
                    float scl = qp2.length()/qp1.length();
                    
                    ofVec2f trans(0.5*(qp1-qp2));
                    if (shouldDecay()) {
                        trans *= 0.5;
                    }

                    transform(anchor,trans,scl ); 
                    
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
        
        if (touches.empty()) {
            if (mat.getScale().x < minZoom) {
                ofMatrix4x4 centerMat=screenMat;
                centerMat.preMult(ofMatrix4x4::newTranslationMatrix(window.getCenter()));
                centerMat.preMult(ofMatrix4x4::newScaleMatrix(minZoom, minZoom, 1.0));
                
                animateBack(centerMat);
            } else if (shouldDecay()) {
                animateBack(gmat);               
            } else if ( velocity.length()>0 && ofGetElapsedTimeMillis()-lastMove<100) {
//                cout << velocity.length() << endl;
                state = STATE_ANIMATING_DRAG;
            }
        
        } 
    }
};

void ofxDragScale::touchDoubleTap(ofTouchEventArgs &touch) {
    
};

void ofxDragScale::touchCancelled(ofTouchEventArgs &touch) {
    
};



void ofxDragScale::update() {
	
    if (state == STATE_ANIMATING_DRAG) {
        if (shouldDecay()) {
            animateBack(gmat);
        } else {
            gmat = mat;
            if (velocity.length()>1) {
                transform(ofVec2f(0,0),velocity/ofGetFrameRate(), 1.0);
                velocity*=0.8;
                
            } else {
                state = STATE_IDLE;
            }
        }
        
		
	}	
    
        
    if (state == STATE_ANIMATING_SCALE) {
        penner.update();
        ofVec2f anchor = imat.preMult(ofVec3f(downPos));
        transform(anchor, ofVec2f(0,0), penner.getParam(PARAM_SCALE)/mat.getScale().x);
        
        if (!penner.getIsEasing(PARAM_SCALE)) {
            state = STATE_IDLE;
        } 
        
    }
    
    if (state == STATE_ANIMATING_BACK) {
        penner.update();
        //ofVec2f anchor = imat.preMult(ofVec3f(downPos));
        transform(ofVec2f(0,0), ofVec2f(penner.getParam(PARAM_X)-mat.getTranslation().x,penner.getParam(PARAM_Y)-mat.getTranslation().y), penner.getParam(PARAM_SCALE)/mat.getScale().x);
        
        if (!penner.getIsEasing(PARAM_X)) {
            state = STATE_IDLE;
        } 
        
    }
    
}

bool ofxDragScale::getIsAnimating() {
    return state!=STATE_IDLE;
};

bool ofxDragScale::shouldDecay() {
    ofVec2f topLeft(worldToScreen(ofVec2f(rect.x,rect.y)));
    ofVec2f bottomRight(worldToScreen(ofVec2f(rect.x+rect.width,rect.y+rect.height)));
    
    return mat.getScale().x>maxZoom || topLeft.x>window.x || topLeft.y>window.y || bottomRight.x<window.x+window.width || bottomRight.y<window.y+window.height;
        
}

void ofxDragScale::transform(ofVec2f anchor,ofVec2f trans,float scl) {
    
        
    
    ofMatrix4x4 temp(mat);
    temp.preMult(ofMatrix4x4::newScaleMatrix(scl, scl, 1.0));  
    
    ofVec2f vec = temp.preMult(ofVec3f(anchor))-mat.preMult(ofVec3f(anchor));
    temp.translate(trans-vec);
    
    mat = temp;
    imat = temp.getInverse();
    
    xformMat.preMult(ofMatrix4x4::newScaleMatrix(scl, scl, 1.0));
    xformMat.translate(trans-vec);
        
    
}


/*
bool ofxDragScale::transform(ofVec2f anchor,ofVec2f trans,float scl) {
    
   
    
    ofMatrix4x4 temp(mat);
    temp.preMult(ofMatrix4x4::newScaleMatrix(scl, scl, 1.0));  
    
    ofVec2f vec = temp.preMult(ofVec3f(anchor))-mat.preMult(ofVec3f(anchor));

    
    temp.translate(trans-vec);
        
    ofMatrix4x4 itemp = temp.getInverse();
       
    if (temp.getScale().x >= minZoom && temp.getScale().x <= maxZoom && rect.inside(itemp.preMult(screenMat.preMult(ofVec3f(window.getCenter()))))) {
        mat = temp;
        imat = itemp;
        xformMat.preMult(ofMatrix4x4::newScaleMatrix(scl, scl, 1.0));
        xformMat.translate(trans-vec);
        return true;
    }
    
    return false;
}
*/

void ofxDragScale::animateScale(ofVec2f pos,float scale) {
    state = STATE_ANIMATING_SCALE;
    penner.setup(PARAM_SCALE, mat.getScale().x);
    penner.start(PARAM_SCALE, EASE_OUT_QUAD, scale);
    downPos = pos;
    
}

void ofxDragScale::animateBack(ofMatrix4x4 target) {
    state = STATE_ANIMATING_BACK;
    penner.setup(PARAM_X, mat.getTranslation().x);
    penner.start(PARAM_X, EASE_OUT_QUAD, target.getTranslation().x);
    penner.setup(PARAM_Y, mat.getTranslation().y);
    penner.start(PARAM_Y, EASE_OUT_QUAD, target.getTranslation().y);
    penner.setup(PARAM_SCALE, mat.getScale().x);
    penner.start(PARAM_SCALE, EASE_OUT_QUAD, target.getScale().x);

}



ofVec3f ofxDragScale::screenToWorld( ofVec3f p ){
	return imat.preMult(p);
}

ofVec3f ofxDragScale::worldToScreen(ofVec3f p) {
    return mat.preMult(p);
}


