//
//  ofxScrollCollection.h
//  scrollCollectionExample
//
//  Created by Roee Kremer on 6/19/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#pragma once

#include "ofMain.h"

struct scrollCollectionPrefs {
    scrollCollectionPrefs() {}; // default constructor
    scrollCollectionPrefs(bool bVertical,ofRectangle rect,float seperator = 10,float inset = 10, float borderSize = 2,int hexBorderColor=0x000000,int selectionDelay=500) :
    bVertical(bVertical),rect(rect),seperator(seperator),inset(inset),borderSize(borderSize),hexBorderColor(hexBorderColor),selectionDelay(selectionDelay)
    {};
    
    ofRectangle rect;
    float seperator;
    float inset;
    float borderSize;
    int hexBorderColor;        
    int selectionDelay;
    bool bVertical;
    
    
};

class ofxScrollCollection {
public:
    void setup(scrollCollectionPrefs prefs);
    void addItem(string filename);
    void update();
    void draw();
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
private:
    float getContentLength();
    vector<ofImage>::reverse_iterator find(ofVec2f touch);
    ofVec2f getVec(float x);
    float getScalar(ofVec2f v);
    ofVec2f degenerate(ofVec2f v);
    
    
    vector<ofImage> images;
    vector<ofImage>::reverse_iterator selected;
   
    scrollCollectionPrefs prefs;
    
    ofVec2f offset;
    
    
    ofTouchEventArgs touch;
    ofVec2f velocity;
    float time;
    
    float downTime;
    vector<ofImage>::reverse_iterator downIter;
    
    int state;
    
    float easeStart;
    float easeTarget;
    
    
    
};
