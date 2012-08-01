//
//  kaiserNav.h
//  kaiserNav
//
//  Created by Roee Kremer on 7/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

#include "ofxPanZoom.h"

#include "ofxBigImage.h"


#include "ofxFlash.h"
#include "ofxFloatingCaption.h"

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
    void setLanguage(string lang);
    
	ofxPanZoom	cam;
    ofxBigImage image;
    	
    ofxDocument doc;
    ofxSymbolInstance interfaceLayout;
    ofxSymbolInstance imageLayout;
    
    bool bCaptionActive;
    ofxSymbolInstance caption;
    ofxFloatingCaption floating;
    
    
    vector<pair<ofxSymbolInstance *,ofxSymbolInstance> > markers;
    
    
    string lang;
    string imageName;
    string captionName;
    float captionLength;
    
        
};
