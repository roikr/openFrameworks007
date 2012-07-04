#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#include "ofxPanZoom.h"
#include "ofxDeepZoom.h"

class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
    
    ofVec2f camOffset();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);

	void lostFocus(){};
	void gotFocus(){};
	void gotMemoryWarning(){};
	void deviceOrientationChanged(int newOrientation);

	ofxPanZoom	cam;
	ofxDeepZoom deep;
};

