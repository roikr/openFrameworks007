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

inline float easeOutQuad(float t, float b, float c, float d) {
    return -c *(t/=d)*(t-2) + b;
};

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
//    imat = prefs.mat.getInverse();
    state = SLIDER_STATE_IDLE;
    counter = 1; // first id is 1
    selected = items.end();
}

deque<itemStruct>::iterator ofxScrollCollection::findByID(int itemID) {
    deque<itemStruct>::iterator iter;
    for (iter=items.begin();iter!=items.end();iter++) {
        if (iter->itemID == itemID) {
            break;
        }
    }
    
    return iter;
}

int ofxScrollCollection::addItem(ofImage &img,string name) {
    
    if (img.getWidth()>0) {
        //bool bSelected = !images.empty() && selected != images.rend();
        int selectedID = getSelectedID();
        items.push_front(itemStruct(img,name,counter));
        counter++;
        if (selectedID) {
            select(selectedID);
        } else {
            deselect();
        }
        return items.front().itemID;
        
    }
    
    return 0;
}

void ofxScrollCollection::removeItem(int itemID) {
    deque<itemStruct>::iterator iter=findByID(itemID);
    if (iter!=items.end()) {
        
        int selectedID = getSelectedID();
        items.erase(iter);
        if (selectedID && selectedID!=itemID) {
            select(selectedID);
        } else {
            deselect();
        }
        
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
                //offset = getVec(easeOutBack(t-time, easeStart, easeTarget-easeStart, EASE_DURATION,4));
                offset = getVec(easeOutQuad(t-time, easeStart, easeTarget-easeStart, EASE_DURATION));
            } else {
                offset = getVec(easeTarget);
                state = SLIDER_STATE_IDLE;
            }
            
        } break;
            
//            case SLIDER_STATE_DOWN:    // select only if down for several ms and before moving
//
//            if (downIter!=images.rend() && (ofGetElapsedTimef()-downTime)*1000 > prefs.selectionDelay) {
//                selected = downIter;
//                downIter = images.rend();
//
//            }
            
        default:
            break;
            
        
            
    }
    
    
}



void ofxScrollCollection::draw() {
    
    
    
    ofVec2f border(prefs.borderSize,prefs.borderSize);
    
    float w =  prefs.width-2*prefs.inset;
    float h =  prefs.height-2*prefs.inset;
    
    ofVec2f pos=ofVec2f(0,0);
    if (prefs.bVertical) {
        pos+=ofVec2f(prefs.inset,offset.y+prefs.seperator);
    } else {
        pos+=ofVec2f(offset.x+prefs.seperator,prefs.inset);
    }
        
    for (deque<itemStruct>::iterator iter=items.begin(); iter!=items.end(); iter++) {
       
        if (prefs.bVertical) {
            h = iter->image.getHeight()/iter->image.getWidth()*w;
        } else {
            w = iter->image.getWidth()/iter->image.getHeight()*h;
        }
        
//        if (iter == selected) {
//            ofSetHexColor(prefs.hexBorderColor);
//            ofRect(pos-border, w+2*prefs.borderSize, h+2*prefs.borderSize);
//        }
        
        ofSetHexColor(0xFFFFFF);
       
//        if (iter->type == OF_IMAGE_COLOR_ALPHA) {
//            ofEnableAlphaBlending();
//        }
        iter->image.draw(pos, w, h);
//        if (iter->type == OF_IMAGE_COLOR_ALPHA) {
//            ofDisableAlphaBlending();
//        }
       
        pos+=degenerate(ofVec2f(w,h)+prefs.seperator*ofVec2f(1,1));
            
        
    }
    
}

void ofxScrollCollection::clear() {
    items.clear();      // roikr: careful - involve with copy ?
    deselect();
}

void ofxScrollCollection::select(int itemID) {
    selected = findByID(itemID);
}

void ofxScrollCollection::deselect() {
    selected = items.end();
    //selected = images.rbegin();
}

bool ofxScrollCollection::getIsSelected() {
    return selected!=items.end();
}

int ofxScrollCollection::getSelectedID() {
    return selected!=items.end() ? selected->itemID : 0;
}

itemStruct &ofxScrollCollection::getItem(int itemID) {
    return *findByID(itemID);
}

bool ofxScrollCollection::getIsDown() {
    return downIter!=items.end();
}

int ofxScrollCollection::getDownID() {
    return downIter!=items.end() ? downIter->itemID : 0;
}

ofRectangle ofxScrollCollection::getRectangle(int itemID) {
    ofRectangle rect;
    
    float w =  prefs.width-2*prefs.inset;
    float h =  prefs.height-2*prefs.inset;
    
    ofVec2f pos;
    if (prefs.bVertical) {
        pos+=ofVec2f(prefs.inset,offset.y+prefs.seperator);
    } else {
        pos+=ofVec2f(offset.x+prefs.seperator,prefs.inset);
    }
    
    for (deque<itemStruct>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if (prefs.bVertical) {
            h = iter->image.getHeight()/iter->image.getWidth()*w;
        } else {
            w = iter->image.getWidth()/iter->image.getHeight()*h;
        }
        
        if (iter->itemID == itemID) {
//            ofVec2f rectPos = prefs.mat.preMult(ofVec3f(pos.x,pos.y));
//            ofVec2f rectMax = prefs.mat.preMult(ofVec3f(pos.x+w,pos.y+h));
//            rect.set(rectPos, rectMax.x-rectPos.x, rectMax.y-rectPos.y);
            rect.set(pos, w, h);
            break;
        }
        
        
        
        pos+=degenerate(ofVec2f(w,h)+prefs.seperator*ofVec2f(1,1));
        
        
    }
    
    return rect;
}

//ofVec2f ofxScrollCollection::screenToWorld(ofVec2f pos) {
//    return imat.preMult(ofVec3f(pos.x,pos.y,0));
//}

void ofxScrollCollection::touchDown(ofTouchEventArgs &touch) {
    
    
    ofVec2f downPos(touch.x,touch.y);
    downIter = items.end();
    if (ofRectangle(0,0,prefs.width,prefs.height).inside(downPos) && state !=SLIDER_STATE_PANNING) {
        state = SLIDER_STATE_DOWN;
        this->touch = touch;
        velocity = ofVec2f(0,0);
        downTime = time = ofGetElapsedTimef();
        downIter = find(downPos);
    }
}

int ofxScrollCollection::getID(ofVec2f pos) {
    deque<itemStruct>::iterator iter = find(pos);
    return iter!=items.end() ? iter->itemID : 0;
    
}

void ofxScrollCollection::touchMoved(ofTouchEventArgs &touch){
    
    ofVec2f movePos(touch.x,touch.y);
    if (state == SLIDER_STATE_IDLE) {
        touchDown(touch);
    } else if ((state==SLIDER_STATE_DOWN || state==SLIDER_STATE_PANNING) && touch.id == this->touch.id)  {
        if (!ofRectangle(0,0,prefs.width,prefs.height).inside(movePos)) {
            touchUp(touch);
        } else {
            float contentLength = getContentLength();
            float rectLength = getScalar(ofVec2f(prefs.width,prefs.height));
            //            float rectPos = getScalar(ofVec2f(prefs.rect.x,prefs.rect.y));
            
            if (rectLength<contentLength) { 
                
                downIter = items.end();
                state = SLIDER_STATE_PANNING;
                ofVec2f posDiff = degenerate(movePos - ofVec2f(this->touch.x,this->touch.y));
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
    }
    
    
}

void ofxScrollCollection::touchUp(ofTouchEventArgs &touch){
    if ((state==SLIDER_STATE_DOWN || state==SLIDER_STATE_PANNING) && touch.id == this->touch.id) {
        
        
        if (downIter!=items.end() && state == SLIDER_STATE_DOWN) {
            selected = downIter;
        }

        
        downIter = items.end();
        
        float contentLength = getContentLength();
        float rectLength = getScalar(ofVec2f(prefs.width,prefs.height));
        time = ofGetElapsedTimef();
        easeStart = getScalar(offset);
        
        if (contentLength>rectLength) { 
            if (getScalar(offset)>0) {
                state = SLIDER_STATE_BACK_TRACKING;
                easeTarget = 0;
            } else if (getScalar(offset)<rectLength-contentLength) {
                state = SLIDER_STATE_BACK_TRACKING;
                easeTarget = rectLength-contentLength;
            } else {
                float v0=velocity.length();
                
                if (v0<STOP_VELOCITY) {
                    state = SLIDER_STATE_IDLE;
                } else {
                    
                    float b= log(DECAY_FACTOR)*ofGetFrameRate(); // because we decay the velocity each frame 
                    float t = log(STOP_VELOCITY/v0)/b; // estimate time for stop
                    float dist = v0/b*(exp(t*b)-1); // estimate stop distance
                    //            cout << v0 << "\t" << t << "\t" << dist << endl;
                    //            float rectPos = getScalar(ofVec2f(prefs.rect.x,prefs.rect.y));
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
                    
            }
        } else {
//            if (getScalar(velocity)>0) {
//                if (getScalar(offset)+dist>rectLength-contentLength) {
//                    state = SLIDER_STATE_BACK_TRACKING;
//                    easeTarget = rectLength-contentLength;
//                } else {
//                    state = SLIDER_STATE_ANIMATING;
//                }
//            } else {
//                if (getScalar(offset)-dist<0) {
//                    state = SLIDER_STATE_BACK_TRACKING;
//                    easeTarget = 0;
//                } else {
//                    state = SLIDER_STATE_ANIMATING;
//                }
//            } 

        }
    }
}


void ofxScrollCollection::touchDoubleTap(ofTouchEventArgs &touch){
    ofVec2f pos(touch.x,touch.y);
   
    if (ofRectangle(0,0,prefs.width,prefs.height).inside(pos)) {
        deque<itemStruct>::iterator iter = find(pos);
        if (iter!=items.end()) {
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
        for (deque<itemStruct>::iterator iter=items.begin(); iter!=items.end(); iter++) {
            contentLength+=iter->image.getHeight()/iter->image.getWidth()*w+prefs.seperator;
        }
    } else {
        for (deque<itemStruct>::iterator iter=items.begin(); iter!=items.end(); iter++) {
            contentLength+=iter->image.getWidth()/iter->image.getHeight()*h+prefs.seperator;
        }
    }
    
    return contentLength;
}



bool ofxScrollCollection::getIsInside(ofVec2f pos) {
    return ofRectangle(0,0,prefs.width,prefs.height).inside(pos);
}

deque<itemStruct>::iterator ofxScrollCollection::find(ofVec2f wpos) {
    
    float w =  prefs.width-2*prefs.inset;
    float h =  prefs.height-2*prefs.inset;
    
    ofVec2f pos=ofVec2f(0,0);
    if (prefs.bVertical) {
        pos+=ofVec2f(prefs.inset,offset.y+prefs.seperator);
    } else {
        pos+=ofVec2f(offset.x+prefs.seperator,prefs.inset);
    }

    
    for (deque<itemStruct>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        if (prefs.bVertical) {
            h = iter->image.getHeight()/iter->image.getWidth()*w;
        } else {
            w = iter->image.getWidth()/iter->image.getHeight()*h;
        }
        
        if (getScalar(wpos)>getScalar(pos) && getScalar(wpos)<getScalar(pos+ofVec2f(w,h))) {
            return iter;
            break;
        }
        pos+=degenerate(ofVec2f(w,h)+prefs.seperator*ofVec2f(1,1));
    }
    
    return items.end();
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

int ofxScrollCollection::getNumItems() {
    return items.size();
}
int ofxScrollCollection::findItemByName(string name) {
    deque<itemStruct>::iterator iter;
    for (iter=items.begin();iter!=items.end();iter++) {
        if (iter->name == name) {
            return iter->itemID;
        }
    }
    
    return 0;
}
