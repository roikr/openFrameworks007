//
//  ofxDeepZoom.h
//  deepZoom
//
//  Created by Roee Kremer on 7/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#import "ofxiPhoneExtras.h"

enum {
    TILE_STATE_INACTIVE,
    TILE_STATE_LOAD,
    TILE_STATE_ACTIVE,
    TILE_STATE_UNLOAD,
    
};



struct tile {
    tile(ofRectangle rect,string filename):rect(rect),filename(filename),bInside(false),bSwap(false),state(TILE_STATE_INACTIVE) {};
    
    ofRectangle rect;
	ofImage image;
    string filename;
    
    int state;
    bool bInside;
    bool bSwap;
};

class ofxiDeepZoom {
    
public:
    ofxiDeepZoom():bThreadRunning(false) {};
    void setup(string filename,string extension,float width, float height,ofRectangle viewport,EAGLSharegroup* sharegroup);
    void start();
    void stop();
    void begin();
    void end();
    void update();
    void transform(ofVec2f offset,float scale);
    void draw();
    void drawDebug();
    bool isThreadRunning();
    
    
private:
    
    bool shouldSwap(tile &t);
    
    ofVec2f offset;
    float scale;
    
    float width,height;
    ofRectangle viewport;
    
    ofRectangle rect; // transformed viewport
    float tilesScale;
   
    
    list<tile> tiles;
    string filename;
    string extension;
    
    void threadedFunction();
    
    EAGLContext* context;
    bool bThreadRunning;
    
};
