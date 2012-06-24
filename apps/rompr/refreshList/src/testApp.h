#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxURLFileLoader.h"

struct item {
    ofImage image;
    ofxMapKitLocation location;
    string img_path;
    int itemID;
    
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
    
    void queryViewLocation();
    void processQuery(ofBuffer &query);
    bool getIsItemExist(int itemID);
    
	    
    ofxiPhoneMapKit mapKit;
    
    // optional callbacks for Map related events
    void regionWillChange(bool animated);
    void regionDidChange(bool animated);
    void willStartLoadingMap();
    void didFinishLoadingMap();
    void errorLoadingMap(string errorDescription);
    
    void urlResponse(ofxHttpResponse &response);
    
    bool bUpdatingRegion;
    bool bQueryLocation;
    float lastQuery;
    
    
    
    list<item> items;
    vector<string> imagesList;
    bool bQuery;
    map<int,int> queue;
    
    vector<pair<string,string> > cookies; // login cookies
    
    int status;   // last status
    string error; // last error
    
    ofImage logo;
};
