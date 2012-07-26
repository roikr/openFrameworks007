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
#include "ofxDeepZoom.h"
#include "ofxFlash.h"
#include "ofxFloatingCaption.h"

class kaiserNav  {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
    
    ofVec2f camOffset();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    void updateMarkers();
    
	ofxPanZoom	cam;
	ofxDeepZoom deep;
    
    ofxDocument doc;
    ofxSymbolInstance layout;
    ofxSymbolInstance image;
    ofxSymbolInstance caption;
    ofxFloatingCaption floating;
    layer *background;
    
    vector<pair<ofxSymbolInstance *,ofxSymbolInstance> > markers;
    
    
    float width;
    float height;
    
    ofMatrix4x4 captionMat;
    
    
};
