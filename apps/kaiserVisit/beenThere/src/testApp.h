#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxScrollCollection.h"
#include "ofxDraggable.h"
#include "ofxFlash.h"

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
    
    void pictureTaken(ofImage &image);
    
    ofxScrollCollection images;
    ofxScrollCollection objects;
    
    int state;
    
    vector<item> items;
   
    ofTouchEventArgs lastTouch;
    bool bTouchObject;
    int objectNum;
    
    ofImage photo;
    
    
    ofxDocument doc;
    ofxSymbolInstance layout;
    layer *background;
    layer *scratch;
    
    ofRectangle imageRect;
    ofMatrix4x4 camMat;
    
    ofxSymbolInstance shareLayout;
    layer *shareBackground;
    layer *shareScratch;
    
    ofRectangle shareRect;
    ofMatrix4x4 shareMat;
    
    
    
    
    
};
