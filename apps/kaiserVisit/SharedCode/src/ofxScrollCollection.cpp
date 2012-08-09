//
//  ofxScrollCollection.cpp
//  scrollCollectionExample
//
//  Created by Roee Kremer on 6/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxScrollCollection.h"

// back easing in - backtracking slightly, then reversing direction and moving to target
// t: current time, b: beginning value, c: change in value, d: duration, s: overshoot amount (optional)
// t and d can be in frames or seconds/milliseconds
// s controls the amount of overshoot: higher s means greater overshoot
// s has a default value of 1.70158, which produces an overshoot of 10 percent
// s==0 produces cubic easing with no overshoot
inline float easeOutBack(float t, float b, float c, float d, float s=1.70158);

inline float easeOutBack(float t, float b, float c, float d, float s) {
    t=t/d-1;
    return c*(t*t*((s+1)*t + s) + 1) + b;
}

#define EASE_DURATION 0.5f
#define DECAY_FACTOR 0.8f
#define STOP_VELOCITY 0.0001f

enum  {
	SLIDER_STATE_IDLE,
    SLIDER_STATE_DOWN,
	SLIDER_STATE_PANNING,
    SLIDER_STATE_ANIMATING,
    SLIDER_STATE_BACK_TRACKING
};


void ofxScrollCollection::setup(scrollCollectionPrefs prefs) {
    this->prefs = prefs;
//    pos = prefs.mat.preMult(ofVec3f(0,0,0));
    imat = prefs.mat.getInverse();
    state = SLIDER_STATE_IDLE;
}

void ofxScrollCollection::addItem(ofImage &img) {
    
    if (img.getWidth()>0) {
        bool bSelected = !images.empty() && selected != images.rend();
        images.push_back(img); 
        selected = images.rend();
    }
}

void ofxScrollCollection::removeItem(int pos) {
    if (pos<images.size()) {
        images.erase(images.begin()+pos);
        selected = images.rend();
    }
}

void ofxScrollCollection::update() {
    switch (state) {
        case SLIDER_STATE_ANIMATING:
            if (velocity.length()>STOP_VELOCITY) {
                float t = ofGetElapsedTimef();
                offset+=velocity*(t-time);
                velocity*=DECAY_FACTOR;
                time = t;
            } else {
                state =  SLIDER_STATE_IDLE;
            }
            break;
            
        case SLIDER_STATE_BACK_TRACKING: {
            float t = ofGetElapsedTimef();
            if (t-time<EASE_DURATION) {
                offset = getVec(easeOutBack(t-time, easeStart, easeTarget-easeStart, EASE_DURATION,4));
            } else {
                offset = getVec(easeTarget);
                state = SLIDER_STATE_IDLE;
            }
            
        } break;
            
            case SLIDER_STATE_DOWN:    // select only if down for several ms and before moving
//            case SLIDER_STATE_PANNING: 
            if (downIter!=images.rend() && (ofGetElapsedTimef()-downTime)*1000 > prefs.selectionDelay) {
                selected = downIter;
                downIter = images.rend();
//                cout << selected->getWidth() << "\t" << selected->getHeight() << endl;
            }
            
        default:
            break;
            
        
            
    }
    
    
}



void ofxScrollCollection::draw() {
    
    ofPushMatrix();
    glMultMatrixf(prefs.mat.getPtr());
    
    ofVec2f border(prefs.borderSize,prefs.borderSize);
    
    float w =  prefs.width-2*prefs.inset;
    float h =  prefs.height-2*prefs.inset;
    
    ofVec2f pos=ofVec2f(0,0);
    if (prefs.bVertical) {
        pos+=ofVec2f(prefs.inset,offset.y+prefs.seperator);
    } else {
        pos+=ofVec2f(offset.x+prefs.seperator,prefs.inset);
    }
        
    for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
       
        if (prefs.bVertical) {
            h = iter->getHeight()/iter->getWidth()*w;
        } else {
            w = iter->getWidth()/iter->getHeight()*h;
        }
        
        if (iter == selected) {
            ofSetHexColor(prefs.hexBorderColor);
            ofRect(pos-border, w+2*prefs.borderSize, h+2*prefs.borderSize);
        }
        
        ofSetHexColor(0xFFFFFF);
       
        if (iter->type == OF_IMAGE_COLOR_ALPHA) {
            ofEnableAlphaBlending();
        }
        iter->draw(pos, w, h);
        if (iter->type == OF_IMAGE_COLOR_ALPHA) {
            ofDisableAlphaBlending();
        }
       
        pos+=degenerate(ofVec2f(w,h)+prefs.seperator*ofVec2f(1,1));
            
        
    }
    
    ofPopMatrix();
}

void ofxScrollCollection::clear() {
    images.clear();      // roikr: careful - involve with copy ?
    selected = images.rbegin();
}

bool ofxScrollCollection::getIsSelected() {
    return selected!=images.rend();
}

int ofxScrollCollection::getSelectedNum() {
    return images.size()-distance(images.rbegin(), selected)-1;
}

ofImage &ofxScrollCollection::getImage(int num) {
    return *(images.rbegin()+num);
}

bool ofxScrollCollection::getIsDown() {
    return downIter!=images.rend();
}

int ofxScrollCollection::getDownNum() {
    return distance(images.rbegin(), downIter);
}

ofRectangle ofxScrollCollection::getRectangle(int num) {
    ofRectangle rect;
    
    float w =  prefs.width-2*prefs.inset;
    float h =  prefs.height-2*prefs.inset;
    
    ofVec2f pos;
    if (prefs.bVertical) {
        pos+=ofVec2f(prefs.inset,offset.y+prefs.seperator);
    } else {
        pos+=ofVec2f(offset.x+prefs.seperator,prefs.inset);
    }
    
    for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
        
        if (prefs.bVertical) {
            h = iter->getHeight()/iter->getWidth()*w;
        } else {
            w = iter->getWidth()/iter->getHeight()*h;
        }
        
        if (distance(images.rbegin(),iter) == num) {
            ofVec2f rectPos = prefs.mat.preMult(ofVec3f(pos.x,pos.y));
            ofVec2f rectMax = prefs.mat.preMult(ofVec3f(pos.x+w,pos.y+h));
            rect.set(rectPos, rectMax.x-rectPos.x, rectMax.y-rectPos.y);
            break;
        }
        
        
        
        pos+=degenerate(ofVec2f(w,h)+prefs.seperator*ofVec2f(1,1));
        
        
    }
    
    return rect;
}

ofVec2f ofxScrollCollection::screenToWorld(ofVec2f pos) {
    return imat.preMult(ofVec3f(pos.x,pos.y,0));
}

void ofxScrollCollection::touchDown(ofTouchEventArgs &touch) {
    
    
    ofVec2f downPos = screenToWorld(ofVec2f(touch.x,touch.y));
    downIter = images.rend();
    if (ofRectangle(0,0,prefs.width,prefs.height).inside(downPos) && state !=SLIDER_STATE_PANNING) {
        state = SLIDER_STATE_DOWN;
        this->touch = touch;
        velocity = ofVec2f(0,0);
        downTime = time = ofGetElapsedTimef();
        downIter = find(downPos);
    }
}

void ofxScrollCollection::touchMoved(ofTouchEventArgs &touch){
    if ((state==SLIDER_STATE_DOWN || state==SLIDER_STATE_PANNING) && touch.id == this->touch.id) {
        downIter = images.rend();
        state = SLIDER_STATE_PANNING;
        ofVec2f posDiff = degenerate(ofVec2f(touch.x,touch.y) - ofVec2f(this->touch.x,this->touch.y));
        offset+=posDiff;
        this->touch = touch;
        float newTime = ofGetElapsedTimef();
                    
        if (newTime-time>0) {
            velocity = posDiff/(newTime-time);
        } else {
            velocity = ofVec2f(0,0);
        }
                    
        time = newTime;
        
    }
}

void ofxScrollCollection::touchUp(ofTouchEventArgs &touch){
    if ((state==SLIDER_STATE_DOWN || state==SLIDER_STATE_PANNING) && touch.id == this->touch.id) {
        downIter = images.rend();
        float v0=velocity.length();
        if (v0>STOP_VELOCITY) {
            
            time = ofGetElapsedTimef();
            easeStart = getScalar(offset);
            
            float b= log(DECAY_FACTOR)*ofGetFrameRate(); // because we decay the velocity each frame 
            float t = log(STOP_VELOCITY/v0)/b; // estimate time for stop
            float dist = v0/b*(exp(t*b)-1); // estimate stop distance
            
//            cout << v0 << "\t" << t << "\t" << dist << endl;
            
            float contentLength = getContentLength();
            float rectLength = getScalar(ofVec2f(prefs.width,prefs.height));
//            float rectPos = getScalar(ofVec2f(prefs.rect.x,prefs.rect.y));
                
            if (rectLength>contentLength) {
                if (getScalar(velocity)>0) {
                    if (getScalar(offset)+dist>rectLength-contentLength) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = rectLength-contentLength;
                    } else {
                        state = SLIDER_STATE_ANIMATING;
                    }
                } else {
                    if (getScalar(offset)-dist<0) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = 0;
                    } else {
                        state = SLIDER_STATE_ANIMATING;
                    }
                } 
            } else {
                if (getScalar(velocity)>0) {
                    if (getScalar(offset)+dist>0) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = 0;
                    } else {
                        state = SLIDER_STATE_ANIMATING;
                    }
                } else {
                    if (getScalar(offset)-dist<rectLength-contentLength) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = rectLength-contentLength;
                    } else {
                        state = SLIDER_STATE_ANIMATING;
                    }
                } 
                
            }
              
        } else {
            state = SLIDER_STATE_IDLE;
        }
    }
}

void ofxScrollCollection::touchDoubleTap(ofTouchEventArgs &touch){
    ofVec2f pos = screenToWorld(ofVec2f(touch.x,touch.y));
   
    if (ofRectangle(0,0,prefs.width,prefs.height).inside(pos)) {
        vector<ofImage>::reverse_iterator iter = find(pos);
        if (iter!=images.rend()) {
            selected = iter;
        }
                
    }

}

void ofxScrollCollection::touchCancelled(ofTouchEventArgs &touch){
    
}

float ofxScrollCollection::getContentLength() {
    float contentLength = prefs.seperator;
    float w =  prefs.width-2*prefs.inset;
    float h =  prefs.height-2*prefs.inset;
    
    
    if (prefs.bVertical) {
        for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
            contentLength+=iter->getHeight()/iter->getWidth()*w+prefs.seperator;
        }
    } else {
        for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
            contentLength+=iter->getWidth()/iter->getHeight()*h+prefs.seperator;
        }
    }
    
    return contentLength;
}



bool ofxScrollCollection::getIsInside(ofVec2f pos) {
    return ofRectangle(0,0,prefs.width,prefs.height).inside(screenToWorld(pos));
}

vector<ofImage>::reverse_iterator ofxScrollCollection::find(ofVec2f wpos) {
    
    float w =  prefs.width-2*prefs.inset;
    float h =  prefs.height-2*prefs.inset;
    
    ofVec2f pos=ofVec2f(0,0);
    if (prefs.bVertical) {
        pos+=ofVec2f(prefs.inset,offset.y+prefs.seperator);
    } else {
        pos+=ofVec2f(offset.x+prefs.seperator,prefs.inset);
    }

    
    for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
        if (prefs.bVertical) {
            h = iter->getHeight()/iter->getWidth()*w;
        } else {
            w = iter->getWidth()/iter->getHeight()*h;
        }
        
        if (getScalar(wpos)>getScalar(pos) && getScalar(wpos)<getScalar(pos+ofVec2f(w,h))) {
            return iter;
            break;
        }
        pos+=degenerate(ofVec2f(w,h)+prefs.seperator*ofVec2f(1,1));
    }
    
    return images.rend();
}

ofVec2f ofxScrollCollection::getVec(float x) {
    return prefs.bVertical ? ofVec2f(0,x) : ofVec2f(x,0);
}

float ofxScrollCollection::getScalar(ofVec2f v) {
    return prefs.bVertical ? v.y : v.x;
}

ofVec2f ofxScrollCollection::degenerate(ofVec2f v) {
    return prefs.bVertical ? ofVec2f(0,v.y) : ofVec2f(v.x,0);
}

scrollCollectionPrefs& ofxScrollCollection::getPrefs()  {
    return prefs;
}

