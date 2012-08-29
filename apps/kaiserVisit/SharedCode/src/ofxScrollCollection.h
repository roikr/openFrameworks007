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
    scrollCollectionPrefs(bool bVertical,float width,float height,float seperator = 10,float inset = 10, float borderSize = 2,int hexBorderColor=0x000000,int selectionDelay=500) :
    bVertical(bVertical),width(width),height(height),seperator(seperator),inset(inset),borderSize(borderSize),hexBorderColor(hexBorderColor),selectionDelay(selectionDelay)
    {};
    
    float seperator;
    float inset;
    float borderSize;
    int hexBorderColor;        
    int selectionDelay;
    bool bVertical;
    float width;
    float height;
    
    
    
};

class ofxScrollCollection {
public:
    void setup(scrollCollectionPrefs prefs);
    void addItem(ofImage &image);
    void removeItem(int pos);
    void update();
    void draw();
    
    void clear();
    void select(int num);
    void deselect();
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    bool getIsSelected();
    int getSelectedNum();
    ofImage &getImage(int num);
    
    bool getIsDown();
    int getDownNum();
    ofRectangle getRectangle(int num);

    bool getIsInside(ofVec2f pos);
    scrollCollectionPrefs& getPrefs();
    
//    ofVec2f screenToWorld(ofVec2f pos);
    
    
private:
    vector<ofImage>::reverse_iterator find(ofVec2f pos); // world pos
    float getContentLength();
    
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
    
//    ofMatrix4x4 imat;
    
};
