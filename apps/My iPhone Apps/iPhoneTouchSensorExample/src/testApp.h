#pragma once



#include "ofMain.h"

#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"


#include "ofxCornerDetector.h"
#include "ofxOsc.h"

//#define HOST "localhost"
#define RECEIVE_PORT 9001
#define SEND_PORT 9000

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

	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
	
private:
	vector<ofPoint> curve;
	bool	bSmooth;
	
	int currentCorner;
	
	ofxOscSender sender;
	ofxCornerDetector detector;
	
	ofxOscReceiver	receiver;
	
	bool bGotHost;

};


