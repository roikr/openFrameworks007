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

enum {
    BITMAP_INSTANCE,
    SHAPE,
    SYMBOL_INSTANCE
    
};


class ofxDocument;
class ofxSymbolInstance;
class ofxSymbolItem;
class ofxBitmapItem;

struct curvePath {
    ofVec2f p0;
    vector<vector<ofVec2f> > segments;
    
};

struct linePath {
    ofVec2f p0;
    vector<ofVec2f> segments;
    bool bClosed;
};

struct ofxBitmapFill {
    ofxBitmapItem *bitmapItem;
    ofMatrix4x4 mat;
    ofRectangle rect;
    string bitmapPath;
};


struct ofxShape {
    vector<curvePath> curve;
    vector<linePath> line;
    
    vector<int> solidColorStroke;
    vector<int> solidColorFill;
    vector<ofxBitmapFill> bitmapFill;
    
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

class ofxBitmapItem {
    
public:
    ofxBitmapItem(string name,string href):name(name),href(href) {};
    void load(float u=1.0,float v=1.0);
    void draw();
    void bind();
    void unbind();
    void release();
    
    int getWidth();
    int getHeight();
    
    
    
    string name;
    string href;
    
private:
    
    
//    int frameRight;
//    int frameBottom;
        
    ofImage image;

#ifdef TARGET_OPENGLES
    ofxiTexture texture;
    int width;
    int height;
    float u,v;
    
#endif
    
};



class ofxSymbolInstance;


struct layer {
    vector<ofxSymbolInstance> instances;
    vector<tlfText> texts;
    
    string name;
};



class ofxSymbolItem {
    
public:    
    
    void setup(ofxDocument *doc);    
    
    ofxSymbolInstance createInstance(string name,ofMatrix4x4 mat=ofMatrix4x4(),float alphaMultiplier = 1.0f,ofVec2f transformationPoint=ofVec2f());
    
    
    string href;
    vector<layer> layers;
    
    ofTrueTypeFont font;
    float lineHeight;
    
    ofxDocument *doc;
    vector<ofxShape> shapes;
    
   
};



class ofxSymbolInstance {
public:
    ofxSymbolInstance():bVisible(true) {};
    void drawLayer(layer *ly);
    void draw();
    vector<ofxSymbolInstance> hitTest(ofVec2f pos);
    vector<ofxSymbolInstance> hitLayer(layer *lyr,ofVec2f pos);
    void update();
    
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
    float alpha;
    
    ofxSymbolItem *symbolItem;
    ofxBitmapItem *bitmapItem;
    int shapeIndex;
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







