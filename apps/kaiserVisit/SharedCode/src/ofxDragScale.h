//
//  ofxDragScale.h
//  draggableExample
//
//  Created by Roee Kremer on 8/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxPenner.h"

class ofxDragScale {
public:
    void setup(ofRectangle window,float width,float height);
    void draw();
    void begin();
    void end();
    
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    bool inside(ofVec2f pos);
    
    void setMinZoom(float min){ minZoom = min;}
	void setMaxZoom(float max){ maxZoom = max;}
	void setTransform(ofMatrix4x4 mat) {this->mat = mat;}
    
    void update(); // update loop for animation
    bool getIsAnimating();
    
    void animateScale(ofVec2f pos,float scale);
	
private:
	
	bool transform(ofVec2f anchor,ofVec2f trans,float scl);
    
    vector<ofTouchEventArgs> touches;
    
    ofMatrix4x4 mat;
    
    
    ofRectangle rect;
    ofRectangle window;
    
    float minZoom;
    float maxZoom;
    
    int state;
	int upTime;
    
    
	
    ofVec2f velocity;
    float time;
    
    float lastTime;
    
    ofxPenner penner;
    ofVec2f downPos;
    
};
