#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxScrollCollection.h"
#include "ofxDraggable.h"
#include "ofxFlash.h"
#include "ofxOsc.h"
#include "ofxiFbo.h"
#include "ofxiMail.h"

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
    void share();
    void sendMail();
    void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    void urlResponse(ofHttpResponse &response);
    void mailComposer(int &result);
    
    
    ofxScrollCollection thumbs;
    ofxScrollCollection objects;
    ofImage image;
    
    int state;
    
    vector<item> items;
   
    ofTouchEventArgs lastTouch;
    bool bTouchObject;
    int objectNum;
    

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
    
    
    ofxOscSender sender;
    ofxOscReceiver receiver;
    int port; // receiver port;
    string url;
    vector<string> images;
    
    ofxiFbo fbo;
    ofTexture tex;
    ofImage shareImage;
    bool bShare;
    ofxiMail mail;
    
};
