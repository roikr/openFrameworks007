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
#include "Settings.h"
#include "ofxiFacebook.h"

struct item {
    int objectID;
    ofxDraggable drag;
    float scale;
};


class testApp : public ofxiPhoneApp{
	
public:
	void setup();
	void update();
	void draw();
    void refresh();
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
    void facebookEvent(ofxFBEventArgs &args);
    
    void lostFocus();
	void gotFocus();
    void launchedWithURL(string url);
    
    
    ofxScrollCollection thumbs;
    ofxScrollCollection objects;
    ofImage image;
    vector<float> scales;
    
    int state;
    
    vector<item> items;
    vector<item> newItem;
    
   
    ofTouchEventArgs lastTouch;
    bool bTouchObject;
    int objectID;
    

    ofxDocument doc;
    ofxSymbolInstance layout;
    ofxSymbolInstance defaultImage;
    
    ofxSymbolInstance *menuInterface;
    ofxSymbolInstance *dragInterface;
    ofxSymbolInstance *shareInterface;
    
    ofRectangle imageRect;
    ofMatrix4x4 camMat;
    
    ofxSymbolInstance shareLayout;
   
    
    ofRectangle shareRect;
    ofMatrix4x4 shareMat;
    
    
    ofxOscSender sender;
    ofxOscReceiver receiver;
    int receiverPort; // receiver port;
    string url;
    
    ofxiFbo fbo;
    ofTexture tex;
    ofImage shareImage;
    bool bShare;
    ofxiMail mail;
    
    ofxiFacebook fb;
    bool bPostImage; // use for calling fb.postImage outside fb event callback (bug due to reentrant ?)
    
    ofMatrix4x4 screenMat;
    
    int actionTime;
    ofxBitmapItem *selectedFrame;
    ofMatrix4x4 menuMat;
    
    
    string lang;
    bool bSuccess;
    
};
