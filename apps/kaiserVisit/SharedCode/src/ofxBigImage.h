//
//  ofxBigImage.h
//  emptyExample
//
//  Created by Roee Kremer on 8/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

//#define USE_PVR



struct tile {
    tile(ofRectangle rect):rect(rect) {};
    
    ofRectangle rect;
	ofImage image;

};

class ofxBigImage {
public:

    void setup(string filename,int size,int width=0,int height=0);
    void load();
    void unload();
    void draw();
    void release();
    int getWidth();
    int getHeight();
    bool getDidLoad();
    
private:
    int size;
    vector<tile> tiles;
    int width;
    int height;
    bool bLoaded;
};
