//
//  ofxPencil.h
//  silentNature
//
//  Created by Roee Kremer on 11/23/12.
//
//

#pragma once

#include "ofMain.h"
#include "ofxStroke.h"

class ofxPencil {
public:
    void setup(ofImage tip,int tipSize,int width,int height);
    void draw();
    void setColor(ofColor color);
    void clear();
    void touch(ofVec2f pos);
private:
    
    ofxStroke stroke;
    int curveSize;
    ofFbo fbo;
    ofColor color;
    
    ofImage tip;
    float tipScale;
    
    
};