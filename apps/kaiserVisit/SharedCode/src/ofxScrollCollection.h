//
//  ofxScrollCollection.h
//  scrollCollectionExample
//
//  Created by Roee Kremer on 6/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#pragma once

#include "ofMain.h"

class ofxScrollCollection {
public:
    void setup(int width,int height,int hMargin,int vMargin);
    void addItem(string filename);
    void draw();
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
private:
    vector<ofImage> images;
    
    int width;
    int height;
    int hMargin;
    int vMargin;
};
