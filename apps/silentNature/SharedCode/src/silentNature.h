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
    CUTOUT_TOOL,
    ERASER_TOOL
};

struct stroke {
    stroke(int tool,ofMatrix4x4 mat, int color):tool(tool),mat(mat),color(color) {};
    int tool;
    int color;
    ofMatrix4x4 mat;
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
    void applyTool(ofVec2f pos);
    void drawTexture(float u,float v);
    
    ofxDocument doc;
    ofxSymbolInstance layout;
    ofxSymbolInstance *canvas;
    
     
    int tool;
    vector<stroke> strokes;

};
