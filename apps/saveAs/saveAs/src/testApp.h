#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxPanZoom.h"
#include "ofxiTexture.h"
#include "ofxiVideoPlayer.h"
#include "ofxFlash.h"

struct videoItem {
    ofRectangle rect;
    
    ofVec2f translation;
    float sx;
    float sy;
    
    float width;
    float height;
    
    
    bool bVisible;
    string path;
    float volume;
};

class testApp : public ofxiPhoneApp {
	
public:
	void setup();
	void update();
	void draw();
    void audioOut( float * output, int bufferSize, int nChannels );
	void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
	void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
    
        
    ofxPanZoom cam;
    
    ofxiVideoPlayer player;
    
    vector<float> zOffsets;
    
    ofVec2f videoPos;
    
    ofxSymbolItem layout;
    ofxDocument doc;

    vector<videoItem> videos;
    vector<videoItem>::iterator current;
    
    map<string,int> layers;
    
};


