#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxiStillCamera.h"
#include "ofxSlider.h"
#include "ofxiVolumeButtons.h"
#include "ofxiMail.h"
#include "ofxiOffScreen.h"

struct card {
    ofImage mask;
    ofImage photo;
    ofBuffer buf;
};

class testApp : public ofxiPhoneApp{
	
public:
	void setup();
	void update();
    void drawCard(card &c);
	void draw();
    void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    void pictureTaken(ofPixels &pixels);
    void volumeButtonPressed(int &button);
    void mailComposer(int &result);
    
    ofxiStillCamera cam;
    vector<card> cards;
    
    ofxSlider slider;
    ofxiVolumeButtons buttons;
    ofxiMail mail;
    ofxiOffScreen offscreen;
    ofPixels pixels;
  
    
};
