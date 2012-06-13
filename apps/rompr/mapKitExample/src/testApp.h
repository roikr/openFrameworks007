#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

class testApp : public ofxiPhoneApp, ofxiPhoneMapKitListener{
	
public:
	void setup();
	void update();
	void draw();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
	
	ofImage bikers;
	ofImage gears;
	ofImage tdf;
	ofImage tdfSmall;
	ofImage transparency;
	ofImage bikeIcon;	
    
    ofxiPhoneMapKit mapKit;
    
    // optional callbacks for Map related events
    void regionWillChange(bool animated);
    void regionDidChange(bool animated);
    void willStartLoadingMap();
    void didFinishLoadingMap();
    void errorLoadingMap(string errorDescription);
    
    bool bUpdatingRegion;
    float scale;
};
