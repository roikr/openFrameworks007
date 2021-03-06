//
//  ofxFlash.h
//  flashExample
//
//  Created by Roee Kremer on 5/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include <map>

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
    ofImage &getImage();
    
    
    
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


struct frame {
    int index;
    int duration;
    vector<ofxSymbolInstance> instances;
    vector<tlfText> texts;
    
};


struct layer {
    vector<frame> frames;  
    int currentFrame;
    int endTime;
    
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
    void drawLayer(layer *ly,float alpha=1.0);
    void draw(float alpha=1.0);
    bool hitTest(ofVec2f pos,vector<ofxSymbolInstance*> &instances);
    bool hitLayer(layer *ly,ofVec2f pos,vector<ofxSymbolInstance*> &instances);
//    void update();
    
    ofRectangle getBoundingBox(ofMatrix4x4 mat = ofMatrix4x4());
    
    ofxSymbolInstance *getChild(string name);
    bool getChildMat(ofxSymbolInstance *child,ofMatrix4x4 &mat);
    layer *getLayer(string name);
    vector<string> listLayers();
    vector<string> listChilds(string name);
    
    void play();
    void stop();
    void gotoAndStop(int frameNum);
    void update();
    
    
    
    string name;
    
    bool bVisible;
    
    vector<layer> layers;
    
    
    
    int type;
    ofMatrix4x4 mat;
    vector<ofVec2f> transformationPoint;
    float alphaMultiplier;
    
    ofxSymbolItem *symbolItem;
    ofxBitmapItem *bitmapItem;
    int shapeIndex;
    
private:
    int getDuration(layer *ly);
    void setFrame(layer *ly,int frameNum);
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







