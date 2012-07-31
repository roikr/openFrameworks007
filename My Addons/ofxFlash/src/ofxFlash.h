//
//  ofxFlash.h
//  flashExample
//
//  Created by Roee Kremer on 5/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

//#ifdef TARGET_OPENGLES
//#include "ofxiTexture.h"
//#endif

enum {
    SYMBOL_INSTANCE,
    BITMAP_INSTANCE
};


class ofxDocument;
class ofxSymbolInstance;
class ofxSymbolItem;

struct curvePath {
    ofVec2f p0;
    vector<vector<ofVec2f> > segments;
    
};

struct linePath {
    ofVec2f p0;
    vector<ofVec2f> segments;
    bool bClosed;
};


struct shape {
    vector<curvePath> curve;
    vector<linePath> line;
    
    vector<int> solidColorStroke;
    vector<int> solidColorFill;
    vector<ofxSymbolInstance> bitmapFill;
    
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

struct ofxBitmapItem {
    string name;
    string href;
    
//    int frameRight;
//    int frameBottom;
//    
    
    
    
    
    ofImage image;
//#ifdef TARGET_OPENGLES
//    ofxiTexture texture;
//    float uWidth; // width and height from 0 to 1
//    float vHeight;
//    
//    //    ofxiTexture texture;
//    float u; // width and height from 0 to 1
//    float v;
//    
//    //#ifndef TARGET_OPENGLES
//    //    ofImage image;
//    //#else
//    //    ofxiTexture texture;
//    //#endif
//    
//#endif
//    float width;
//    float height;
    
};



class ofxSymbolInstance;


struct layer {
    vector<ofxSymbolInstance> instances;
    vector<shape> shapes;
    vector<tlfText> texts;
    
    string name;
};



class ofxSymbolItem {
    
public:    
    
    void setup(ofxDocument *doc);    
    
    ofxSymbolInstance createInstance(string name,ofMatrix4x4 mat=ofMatrix4x4(),float alpha = 1.0f,ofVec2f transformationPoint=ofVec2f());
    
    
    string href;
    vector<layer> layers;
    
    ofTrueTypeFont font;
    float lineHeight;
    
    ofxDocument *doc;
    
   
};



class ofxSymbolInstance {
public:
    ofxSymbolInstance():bVisible(true) {};
    void bitmapFill(ofxSymbolInstance &instance);
    void drawLayer(layer *ly);
    void draw();
    vector<ofxSymbolInstance> hitTest(ofVec2f pos);
    vector<ofxSymbolInstance> hitLayer(layer *lyr,ofVec2f pos);
    
    ofRectangle getBoundingBox();
    
    ofxSymbolInstance *getChild(string name);
    layer *getLayer(string name);
    vector<string> listLayers();
    vector<string> listChilds(string name);
    string name;
    
    bool bVisible;
    
    vector<layer> layers;
    
    
    
    int type;
    ofMatrix4x4 mat;
    vector<ofVec2f> transformationPoint;
    float alphaMultiplier;
    
    ofxSymbolItem *symbolItem;
    ofxBitmapItem *bitmapItem;
};





class ofxDocument {
    
public:
    void setup(string name); 
    void load();
    void release();
    
    ofxSymbolItem* getSymbolItem(string name);
    ofxBitmapItem* getBitmapItem(string name);
    

private:
    
    vector<ofxSymbolItem> symbolItems; 
    vector<ofxBitmapItem> bitmapItems;
        
//    ofVec2f offset;
//    float zoom;
};







