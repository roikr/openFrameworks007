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
    void setup(ofRectangle window,float width,float height,ofMatrix4x4 mat=ofMatrix4x4(),ofMatrix4x4 screenMat=ofMatrix4x4());
    void draw();
    void begin();
    void end();
    
    void debugDraw();
    
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    bool inside(ofVec2f pos);
    
    void setMinZoom(float minZoom);
	void setMaxZoom(float maxZoom);
	
    
    void update(); // update loop for animation
    bool getIsAnimating();
    
    void animateScale(ofVec2f pos,float scale);
    void animateBack(ofMatrix4x4 target);
    
    ofVec3f screenToWorld(ofVec3f p);
    ofVec3f worldToScreen(ofVec3f p);
    float getScale() {return mat.getScale().x/screenMat.getScale().x;}
    ofMatrix4x4 &getTransform() {return xformMat;}
	
private:
	
    bool shouldDecay(ofMatrix4x4 mat);
	void transform(ofVec2f trans,float scl,ofVec2f anchor=ofVec2f(0,0));
    
    vector<ofTouchEventArgs> touches;
    
    ofMatrix4x4 mat;
    ofMatrix4x4 xformMat;
    ofMatrix4x4 imat;
    
    ofMatrix4x4 screenMat;
    
    
    ofRectangle rect;
    ofRectangle window;
    
    float minZoom;
    float maxZoom;
    
    int state;
	int upTime;
    
    
	
    ofVec2f translation;
    bool bMoving;
    int startMove;
    int lastMove;
    ofVec2f velocity;
    
    
    ofxPenner penner;    
    ofMatrix4x4 gmat;
    
    
    
};
