#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxURLFileLoader.h"

struct item {
    ofImage image;
    double longitude;
    double latitude;
    string img_path;
    
};

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
    
    void processQuery(ofBuffer &query);
	    
    ofxiPhoneMapKit mapKit;
    
    // optional callbacks for Map related events
    void regionWillChange(bool animated);
    void regionDidChange(bool animated);
    void willStartLoadingMap();
    void didFinishLoadingMap();
    void errorLoadingMap(string errorDescription);
    
    void urlResponse(ofxHttpResponse &response);
    
    bool bUpdatingRegion;
    
    
    
    vector<item> items;
    vector<string> list;
    bool bQuery;
    map<int,int> queue;
    
    int status;   // last status
    string error; // last error
    
    ofImage logo;
};
