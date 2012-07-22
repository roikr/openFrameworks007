#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxAudioFile.h"

struct button {
    ofRectangle rect;
    ofxAudioFile audio;
    string text;
};

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
	
	void audioRequested( float * output, int bufferSize, int nChannels );

	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);

	vector<button> buttons;
    ofTrueTypeFont font;
    ofImage background;
    ofxiPhoneKeyboard *keyboard;
    int downTime;
    
    bool bEditMode;
    int keyNum;
    bool bButtonDown;
    
    deque<int> queue;
};


