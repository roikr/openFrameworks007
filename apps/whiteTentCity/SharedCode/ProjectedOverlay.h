//
//  ProjectedOverlay.h
//  tentsMapping
//
//  Created by Roee Kremer on 3/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofVec3f.h"
#include "ofMatrix4x4.h"

struct screen {
    int index; // index of the first vertex of the screen for deleting by first index
    vector<int> indices;
    ofVec3f origin;
    ofVec3f xVec;
    ofVec3f yVec;
    vector<ofVec2f> proj;
    ofMatrix4x4 glMat;
    
    
} ;

struct tent {
    vector<int> indices;
    vector<ofVec3f> vertices;
    ofMatrix4x4 glMat;
};


class ProjectedOverlay {
public:
    ProjectedOverlay(float scale):filename("settings.xml"),scale(scale) {};
    
    void load();
    void save();
    void log();
  
    vector<screen> screens;
    vector<tent> tents;
    
    void updateMatrices();
    
    float scale;
    string filename;
};