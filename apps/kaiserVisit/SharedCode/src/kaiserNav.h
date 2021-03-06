//
//  kaiserNav.h
//  kaiserNav
//
//  Created by Roee Kremer on 7/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

#include "ofxDragScale.h"

#include "ofxBigImage.h"


#include "ofxFlash.h"
#include "ofxFloatingCaption.h"

#ifdef TARGET_OF_IPHONE
#include "ofxiVideoPlayer.h"
#endif

struct dragScale {
    string name;
    float minZoom;
    float zoomOut;
    float maxZoom;
    float zoomIn;
    float zoom;
    ofVec2f offset;
};


class kaiserNav  {
	
public:
	void setup();
	void update();
	void draw();
    void draw2nd();
	void exit();
    
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    void setImage(string name);
    void updateOverlays();
    void setCaption(string name);
    void setState(int state);
   
	ofxDragScale cam;
    
    ofxDocument doc;
    ofxSymbolInstance interfaceLayout;
    ofxSymbolInstance imageLayout;
    
    bool bCaptionActive;
    ofxSymbolInstance caption;
    ofxSymbolInstance extCaption;
    ofxFloatingCaption floating;
    
    
    vector<pair<ofxSymbolInstance *,ofxSymbolInstance> > markers;
    ofxSymbolInstance extMarker;
    ofxSymbolInstance screenLayout;
   
    
    string lang;
    string imageName;
    string captionName;
    bool bSubTitle;
    ofxBitmapItem *title;
    ofxBitmapItem *subTitle;
    ofxBitmapItem *extSubTitle;
    ofMatrix4x4 titleMat;
    ofMatrix4x4 subTitleMat;
    ofMatrix4x4 extTitleMat;
    ofMatrix4x4 extSubTitleMat;
    
    int state;
    int timer;
 
#ifdef TARGET_OF_IPHONE
    ofxiVideoPlayer player;
#endif
    ofMatrix4x4 videoMat;

    
    vector<dragScale> settings;
    vector<ofxBigImage > images;
    map<string,int> imagesMap;
    int imageNum;
   
    ofMatrix4x4 extMat;
    ofVec2f linep1;
    ofVec2f linep2;
        
};
