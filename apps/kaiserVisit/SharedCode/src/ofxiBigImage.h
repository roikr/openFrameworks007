//
//  ofxiBigImage.h
//  kaiserNav
//
//  Created by Roee Kremer on 9/9/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"


#include "ofxiTexture.h"


struct tile {
    tile(ofRectangle rect):rect(rect) {};
    ofRectangle rect;
    ofxiTexture texture;

};

class ofxiBigImage {
public:

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

