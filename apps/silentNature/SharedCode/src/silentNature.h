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
#ifdef TARGET_OPENGLES
#include "ofxiFbo.h"
#endif

#include "ofxHttpUtils.h"

#include "ofxEraser.h"
#include "ofxPencil.h"
#include "ofxSimpleBrush.h"

enum {
    BRUSH_TOOL,
    CRAYON_TOOL,
    CUTOUT_TOOL,
    ERASER_TOOL
};

//struct stroke {
//    stroke(int tool,ofMatrix4x4 mat, int color):tool(tool),mat(mat),color(color) {};
//    int tool;
//    int color;
//    ofMatrix4x4 mat;
//};

class silentNature {
public:   
    void setup();
	void update();
	void draw();
	void exit();
    void clear();
    void publish();
	
	void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    void updateTool();
    void resetTools();
    
    void newResponse(ofxHttpResponse & response);
    
    ofxDocument doc;
    ofxSymbolInstance layout;
    ofxSymbolInstance *canvas;
    ofxSymbolInstance frame;
    ofxSymbolInstance mask;
    
    int tool;
    int paperNum;
    
#ifdef TARGET_OPENGLES
    ofxiFbo fbo;
    ofTexture canvasTex;
#else
    ofFbo fbo;
#endif   
    
    ofMatrix4x4 cmat;
    
    bool bDown;
    
    ofxHttpUtils httpUtils;
    int counter;
   
    int publishTimer;
    ofMatrix4x4 previewMat;
    ofFbo publishFbo;
    ofMatrix4x4 publishMat;
    
    int state;
    
    ofxPencil crayon;
    ofxSimpleBrush brush;
    ofxEraser eraser;
    
    bool bPublish;
    
    string url;
    string prefix;
    
    ofVideoPlayer video;
    int idle;
    int idleTimer;
};
