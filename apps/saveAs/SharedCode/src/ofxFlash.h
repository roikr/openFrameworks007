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

struct bitmapItem {
    string href;
    int frameRight;
    int frameBottom;
};

class ofxDocument {
    
public:
    void setup(string name); 
    map<string, bitmapItem > items;
};

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
    
    string path;
    string href;
    
    ofMatrix4x4 mat;
    
    ofVec2f translation;
    float sx;
    float sy;
    float r;
    
    //    ofxiTexture texture;
    float u; // width and height from 0 to 1
    float v;
    
#ifndef TARGET_OPENGLES
    ofImage image;
#else
    ofxiTexture texture;
#endif
    float width;
    float height;
    
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



struct bitmapInstance {
    
    string libraryItemName;
    
    ofVec2f translation;
    float scale;
    float rotation;
    
    ofVec2f transformationPoint;
    
    //    ofxiTexture texture;
    float uWidth; // width and height from 0 to 1
    float vHeight;
    
#ifndef TARGET_OPENGLES
    ofImage image;
#else
    ofxiTexture texture;
#endif
    
    float width;
    float height;
};

struct layer {
    vector<bitmapInstance> bitmaps;  
    vector<shape> shapes;
    vector<tlfText> texts;
    
    string name;
};



class ofxSymbolItem {

public:    
    void setup(string name,ofxDocument &doc);
    void load();
    
    void drawBitmap(bitmap &bm,ofVec2f offset,float zoom);
    void drawLayer(layer &ly,ofVec2f offset,float zoom);
    void draw(ofVec2f offset = ofVec2f(0,0),float zoom = 1);
    


    void release();
    
    vector<layer> layers;
   
    
    ofTrueTypeFont font;
    float lineHeight;
   
};

