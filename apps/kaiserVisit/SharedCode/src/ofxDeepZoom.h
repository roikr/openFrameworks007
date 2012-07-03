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

struct tile {
    tile(ofRectangle rect):rect(rect),bInside(false) {};
    
    ofRectangle rect;
	ofImage image;
    
    bool bInside;
};

class ofxDeepZoom : protected ofThread {
    
public:
    void setup(string filename,float width, float height,ofRectangle viewport);
    void start();
    void stop();
    void update(ofVec2f offset,float scale);
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
