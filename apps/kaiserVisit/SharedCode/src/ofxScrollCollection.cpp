//
//  ofxScrollCollection.cpp
//  scrollCollectionExample
//
//  Created by Roee Kremer on 6/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxScrollCollection.h"

enum  {
	SLIDER_STATE_IDLE,
    SLIDER_STATE_DOWN,
	SLIDER_STATE_PANNING,
    SLIDER_STATE_ANIMATING,
    SLIDER_STATE_BACK_TRACKING
};


void ofxScrollCollection::setup(ofRectangle rect,int hMargin,int vMargin) {
    this->rect = rect;
    this->hMargin = hMargin;
    this->vMargin = vMargin;
    contentWidth = rect.width;
    contentHeight = vMargin;
    offset = ofVec2f(0,0);
    state = SLIDER_STATE_IDLE;
}

void ofxScrollCollection::addItem(string filename) {
    ofImage img;
    images.push_back(img);
    ofImage &back = images.back();
    back.loadImage(filename);
    
    float h = back.getHeight()/back.getWidth()*(rect.width-2*hMargin);
    contentHeight+=h+vMargin;
    
    selected = images.rbegin();
}

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
                offset.y = easeOutBack(t-time, easeStart, easeTarget-easeStart, EASE_DURATION,4);
            } else {
                offset.y = easeTarget;
                state = SLIDER_STATE_IDLE;
            }
            
        } break;
            
        default:
            break;
            
        
            
    }
    
    
}



void ofxScrollCollection::draw() {
    
    float w =  rect.width-2*hMargin;
    
    int y=offset.y+vMargin;
    for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
        if (iter->getWidth()>0) {
            float h = iter->getHeight()/iter->getWidth()*w;
            
            if (iter == selected) {
                ofSetHexColor(0xFF0000);
                ofRect(hMargin-5, y-5, w+10, h+10);
            }
            
            ofSetHexColor(0xFFFFFF);
           
            if (iter->type == OF_IMAGE_COLOR_ALPHA) {
                ofEnableAlphaBlending();
            }
            iter->draw(hMargin,y, w, h);
            if (iter->type == OF_IMAGE_COLOR_ALPHA) {
                ofDisableAlphaBlending();
            }
            y+=h+vMargin;
            
        }
    }
}

void ofxScrollCollection::touchDown(ofTouchEventArgs &touch) {
    if (rect.inside(ofVec2f(touch.x,touch.y)) && state !=SLIDER_STATE_PANNING) {
        state = SLIDER_STATE_DOWN;
        this->touch = touch;
        velocity = ofVec2f(0,0);
        time = ofGetElapsedTimef();
    }
}

void ofxScrollCollection::touchMoved(ofTouchEventArgs &touch){
    if ((state==SLIDER_STATE_DOWN || state==SLIDER_STATE_PANNING) && touch.id == this->touch.id) {
        
        state = SLIDER_STATE_PANNING;
        ofVec2f posDiff = ofVec2f(touch.x,touch.y) - ofVec2f(this->touch.x,this->touch.y);
        offset+=ofVec2f(0,posDiff.y);
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
    if (touch.id == this->touch.id) {
        float v0=velocity.length();
        if (v0>STOP_VELOCITY) {
            
            time = ofGetElapsedTimef();
            easeStart = offset.y;;
            
            float b= log(DECAY_FACTOR)*ofGetFrameRate(); // decay each frame
            float t = log(STOP_VELOCITY/v0)/b;
            float dist = v0/b*(exp(t*b)-1);
            
//            cout << v0 << "\t" << t << "\t" << dist << endl;
                
            if (rect.height>contentHeight) {
                if (velocity.y>0) {
                    if (offset.y+dist>rect.height-contentHeight) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = rect.height-contentHeight;
                    } else {
                        state = SLIDER_STATE_ANIMATING;
                    }
                } else {
                    if (offset.y-dist<0) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = 0;
                    } else {
                        state = SLIDER_STATE_ANIMATING;
                    }
                } 
            } else {
                if (velocity.y>0) {
                    if (offset.y+dist>0) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = 0;
                    } else {
                        state = SLIDER_STATE_ANIMATING;
                    }
                } else {
                    if (offset.y-dist<rect.height-contentHeight) {
                        state = SLIDER_STATE_BACK_TRACKING;
                        easeTarget = rect.height-contentHeight;
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
    if (rect.inside(ofVec2f(touch.x,touch.y))) {
        
        float w =  rect.width-2*hMargin;
        
        int y=offset.y+vMargin;
        for (vector<ofImage>::reverse_iterator iter=images.rbegin(); iter!=images.rend(); iter++) {
            if (iter->getWidth()>0) {
                float h = iter->getHeight()/iter->getWidth()*w;
                
                if (touch.y>y && touch.y<y+h) {
                    selected = iter;
                    break;
                }
                y+=h+vMargin;
                
            }
        }
        
    }

}

void ofxScrollCollection::touchCancelled(ofTouchEventArgs &touch){
    
}

