//
//  ofxFlash.h
//  flashExample
//
//  Created by Roee Kremer on 5/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

#ifdef TARGET_OPENGLES
#include "ofxiTexture.h"
#endif

#define PIXEL_SCALE 20.0

class ofxDocument;

struct curvePath {
    ofVec2f p0;
    vector<vector<ofVec2f> > segments;
    
};

struct linePath {
    ofVec2f p0;
    vector<ofVec2f> segments;
    bool bClosed;
};

struct bitmap {
    
//    string path;
    int itemID;
//    string href;
    
    ofMatrix4x4 mat;
    
    ofVec2f translation;
    float sx;
    float sy;
    float r;
        
    ofRectangle rect;
};

struct shape {
    vector<curvePath> curve;
    vector<linePath> line;
    
    vector<int> solidColorStroke;
    vector<int> solidColorFill;
    vector<bitmap> bitmapFill;
    
};

struct span {
    string text;
    int fontSize;
    int color;
};

struct tlfText {
    
    vector<span> spans;
    string text;
    
    ofVec2f translation;
    float sx;
    float sy;
    float r;
    
    float width;
    float height;

};

enum {
    SYMBOL_INSTANCE,
    BITMAP_INSTANCE
};

struct instance {

    string name;
    int itemID;
    ofVec2f translation;
    float scale;
    float rotation;
    
    ofVec2f transformationPoint;
    
    int type;
};

struct layer {
    vector<instance> instances;
    vector<shape> shapes;
    vector<tlfText> texts;
    
    string name;
};



class ofxSymbolItem {

public:    
    
    void setup(ofxDocument *doc);
    ofRectangle getScreenRect(ofRectangle& rect);
    
    void drawBitmap(bitmap &bm);
    void drawLayer(layer &ly);
    void draw();
    
    void hitTest(ofVec2f pos);
    
    string href;
    vector<layer> layers;
   
    ofTrueTypeFont font;
    float lineHeight;
    
    ofxDocument *doc;
    
    
};


struct bitmapItem {
    string name;
    string href;
    
    int frameRight;
    int frameBottom;
    
    
    //    ofxiTexture texture;
    float u; // width and height from 0 to 1
    float v;
    
#ifndef TARGET_OPENGLES
    ofImage image;
#else
    ofxiTexture texture;
    float uWidth; // width and height from 0 to 1
    float vHeight;
    
    //#ifndef TARGET_OPENGLES
    //    ofImage image;
    //#else
    //    ofxiTexture texture;
    //#endif
#endif
    float width;
    float height;
};


class ofxDocument {
    
public:
    void setup(string name); 
    void load();
    void release();
    
    vector<bitmapItem> bitmapItems;
    vector<ofxSymbolItem> symbolItems; 
    map<string, int> itemsMap;
    
    ofVec2f offset;
    float zoom;
};







