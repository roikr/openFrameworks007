#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxScrollCollection.h"
#include "ofxDraggable.h"

struct item {
    int objectNum;
    ofxDraggable drag;
    float scale;
};

class testApp : public ofxiPhoneApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
	
    ofxScrollCollection images;
    ofxScrollCollection objects;
    
    int state;
    
    vector<item> items;
   
    ofTouchEventArgs lastTouch;
    bool bTouchObject;
    int objectNum;
    
    ofImage scratch;
    ofImage photo;
    
    
};
