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
    void setup(ofRectangle rect,int hMargin,int vMargin);
    void addItem(string filename);
    void update();
    void draw();
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
private:
    
    vector<ofImage> images;
    vector<ofImage>::reverse_iterator selected;
   
    ofRectangle rect;
    float contentWidth;
    float contentHeight;
    ofVec2f offset;
    int hMargin;
    int vMargin;
    
    ofTouchEventArgs touch;
    ofVec2f velocity;
    float time;
    
    int state;
    
    float easeStart;
    float easeTarget;
    
    
    
};
