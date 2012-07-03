//
//  ofxDeepZoom.h
//  deepZoom
//
//  Created by Roee Kremer on 7/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

#define TILE_SIZE 256

enum {
    TILE_STATE_QUEUE,
    TILE_STATE_LOAD,
    TILE_STATE_ACTIVE,
    TILE_STATE_SWAP,
    TILE_STATE_UNLOAD,
    TILE_STATE_DELETE
};

struct tile {
    tile(ofRectangle rect,string filename):rect(rect),filename(filename),bInside(false),state(TILE_STATE_QUEUE) {};
    
    ofRectangle rect;
	ofImage image;
    string filename;
    
    int state;
    bool bInside;
};

class ofxDeepZoom : protected ofThread {
    
public:
    void setup(string filename,string extension,float width, float height,ofRectangle viewport);
    void start();
    void stop();
    void begin();
    void end();
    void update();
    void transform(ofVec2f offset,float scale);
    void draw();
    
private:
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
    
};
