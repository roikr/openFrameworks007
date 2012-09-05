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

struct itemStruct  {
    itemStruct(ofImage &image,string name,int itemID):image(image),name(name),itemID(itemID) {};
    ofImage image;
    string name;
    int itemID;
};

class ofxScrollCollection {
public:
    void setup(scrollCollectionPrefs prefs);
    int addItem(ofImage &image,string name="");
    void removeItem(int itemID);
    void update();
    void draw();
    
    void clear();
    void select(int itemID);
    void deselect();
    
    void touchDown(ofTouchEventArgs &touch);
	void touchMoved(ofTouchEventArgs &touch);
	void touchUp(ofTouchEventArgs &touch);
	void touchDoubleTap(ofTouchEventArgs &touch);
	void touchCancelled(ofTouchEventArgs &touch);
    
    bool getIsSelected();
    int getSelectedID();
    itemStruct &getItem(int itemID);
    
    
    bool getIsDown();
    int getDownID();
    ofRectangle getRectangle(int itemID);

    bool getIsInside(ofVec2f pos);
    scrollCollectionPrefs& getPrefs();
    int getNumItems();
    int findItemByName(string name);
    
//    ofVec2f screenToWorld(ofVec2f pos);
    
    
private:
    deque<itemStruct>::iterator find(ofVec2f pos); // world pos
    deque<itemStruct>::iterator findByID(int itemID);
    
    float getContentLength();
    
    ofVec2f getVec(float x);
    float getScalar(ofVec2f v);
    ofVec2f degenerate(ofVec2f v);
    
    
    deque<itemStruct> items;
    deque<itemStruct>::iterator selected;
   
   
    scrollCollectionPrefs prefs;
    
    ofVec2f offset;
    
    
    ofTouchEventArgs touch;
    ofVec2f velocity;
    float time;
    
    float downTime;
    deque<itemStruct>::iterator downIter;
    
    int state;
    
    float easeStart;
    float easeTarget;
    
    int counter;
    
//    ofMatrix4x4 imat;
    
};
