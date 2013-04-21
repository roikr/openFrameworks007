#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxiStillCamera.h"
#include "ofxSlider.h"
#include "ofxiVolumeButtons.h"
#include "ofxiOffScreen.h"
#include "ofxUpload.h"

enum {
    STATE_MEMORY_UNLOADED,
    STATE_MEMORY_LOADED,
    STATE_TEXTURE_LOADED,
    STATE_TEXTURE_UNLOADED,
};

struct card {
    string filename;
    int state;
    ofImage image;
    ofSoundPlayer audio;
    bool bMask;
   
};

class testApp : public ofxiPhoneApp,protected ofThread{
	
public:
	void setup();
	void update();
   
	void draw();
    void exit();
    
    void threadedFunction();
    
    void drawCard(card &c);
    void toggleAudio();
    void startCamera();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    void pictureTaken(ofPixels &pixels);
    void volumeButtonPressed(int &button);
    void newResponse(ofxHttpResponse & response);
    
    ofxiStillCamera cam;
    vector<card> cards;
    
    ofxSlider slider;
    ofxiVolumeButtons buttons;
    ofxiOffScreen offscreen;
    ofPixels pixels;
    
    ofImage photo;
    ofBuffer buf;
    ofxUpload upload;
    string action_url;
    int counter;
    
    int lastPage;
    
    ofVideoPlayer player;
   
};
