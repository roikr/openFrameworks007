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
    TILE_STATE_LOAD,
    TILE_STATE_ACTIVE,
    TILE_STATE_SWAP,
    TILE_STATE_UNLOAD,
};

struct tile {
    tile(ofRectangle rect):rect(rect),bInside(false),state(TILE_STATE_LOAD) {};
    
    ofRectangle rect;
	ofImage image;
    
    int state;
    bool bInside;
};

class ofxDeepZoom : protected ofThread {
    
public:
    void setup(string filename,float width, float height,ofRectangle viewport);
    void start();
    void stop();
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
    
};
