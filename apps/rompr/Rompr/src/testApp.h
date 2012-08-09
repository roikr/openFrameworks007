#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxURLFileLoader.h"
#include "ofxPenner.h"
#include "ofxTransform.h"
#include "ofxiStillCamera.h"
#include "ofxiVolumeButtons.h"
#include "ofxiFacebook.h"



struct item {
    item():bQueued(false) {}
    
    ofImage image;
    ofImage thumb;
    ofxMapKitLocation location;
    
    string img_path;
    int itemID;
    bool bQueued;
    
    int count;
    ofVec2f gridPos;
    float angle;
    
    ofRectangle rect;
};

class testApp : public ofxiPhoneApp, ofxiPhoneMapKitListener {
	
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
    list<item>::iterator findItem(int itemID);
    void updateItems(vector<item> newItems);
    void calcItems();
    
    void lostFocus();
	void gotFocus();
	void gotMemoryWarning();
	void deviceOrientationChanged(int newOrientation);
    void launchedWithURL(string url);
    
    
    void showRecommendation(string html);
    void hideRecommendation();
    void urlResponse(ofxHttpResponse &response);
    
    ofVec2f getScreenCoordinatesForLocation(ofxMapKitLocation location);
    ofxMapKitLocation getUserLocation();
	    
    ofxiPhoneMapKit mapKit;
    ofxiPhoneCoreLocation coreLocation;
    ofxMapKitLocation userLocation;
    bool bStartLocation;
    
    // optional callbacks for Map related events
    void regionWillChange(bool animated);
    void regionDidChange(bool animated);
    void willStartLoadingMap();
    void didFinishLoadingMap();
    void errorLoadingMap(string errorDescription);
    
    void volumeButtonPressed(int &button);
    void pictureTaken(ofPixels &pixels);
    
    bool bUpdatingRegion;
    bool bQueryLocation;
    float lastQuery;
    
    
    
    list<item> items;
    int nearestID;
    
    
    
    vector<int> downloads;
    bool bQuery;
    map<int,int> queue;
    
    vector<pair<string,string> > cookies; // login cookies
    
    int status;   // last status
    string error; // last error
    
    ofImage logo;
    ofxTransform imageXform;
    ofxTransform cameraXform;
    
    bool bSelected;
    int selectedID;
    bool bDeselect;
    bool bRecommendation;
    
    
   
    int state;
    
    ofxiStillCamera cam;
    ofxiVolumeButtons volumeButtons;
    bool bStartCamera;
    bool bStopCamera;
    
    ofImage image;
    ofxMapKitLocation imageLocation;
    
    
    ofxPenner penner;
    ofxiFacebook fb; 
    bool bLogin;
    
};
