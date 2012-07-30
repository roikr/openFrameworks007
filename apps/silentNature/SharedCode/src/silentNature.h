//
//  silentNature.h
//  iSilentNature
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxFlash.h"

enum {
    BRUSH_TOOL,
    CRAYON_TOOL,
    CUTOUT_TOOL
};

class silentNature {
public:   
    void setup();
	void update();
	void draw();
	void exit();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    void setTool(int tool);
    
    ofxDocument doc;
    ofxSymbolInstance layout;
    
    int tool;

};
