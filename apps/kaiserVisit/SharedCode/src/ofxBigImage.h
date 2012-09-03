//
//  ofxBigImage.h
//  emptyExample
//
//  Created by Roee Kremer on 8/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

#ifdef TARGET_OPENGLES
#include "ofxiTexture.h"
#endif

struct tile {
    tile(ofRectangle rect):rect(rect) {};
    
    ofRectangle rect;
#ifdef TARGET_OPENGLES
    ofxiTexture texture;
#else
	ofImage image;
#endif
};

class ofxBigImage {
public:
#ifndef TARGET_OPENGLES
    void loadImage(ofImage image,int size);
    
#endif
    void loadImage(string filename,int size,int width=0,int height=0);
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
};
