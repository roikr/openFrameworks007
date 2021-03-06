#pragma once

#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "kaiserNav.h"
#include "ofxiExternalView.h"

class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
    
    
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);

	void lostFocus(){};
	void gotFocus(){};
	void gotMemoryWarning(){};
	void deviceOrientationChanged(int newOrientation);

	kaiserNav nav;   
    ofxiExternalView extView;
    
};


