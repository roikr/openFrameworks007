//
//  ofxBigImage.h
//  emptyExample
//
//  Created by Roee Kremer on 8/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

struct tile {
    tile(ofRectangle rect):rect(rect) {};
    
    ofRectangle rect;
	ofImage image;
};

class ofxBigImage {
public:
    void loadImage(string filename,int size);
    void draw();
    int getWidth();
    int getHeight();
    
private:
    int size;
    vector<tile> tiles;
    int width;
    int height;
};
