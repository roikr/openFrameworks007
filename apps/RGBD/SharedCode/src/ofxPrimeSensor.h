//
//  ofxPrimeSensor.h
//  primeSensor
//
//  Created by Roee Kremer on 6/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <XnOS.h>

//#include <math.h>

#include <XnCppWrapper.h>
#include "ofMain.h"

using namespace xn;


class ofxPrimeSensor {
public:
    void setup();
    void update();
   
    ofPixels &getDepthPixels();
    ofPixels &getImagePixels();
    
private:
    
    
    float* g_pDepthHist;
    XnDepthPixel g_nZRes;
    
    Context g_context;
    ScriptNode g_scriptNode;
    DepthGenerator g_depth;
    ImageGenerator g_image;
    DepthMetaData g_depthMD;
    ImageMetaData g_imageMD;
    
    
    
    ofPixels depthPixels;
    ofPixels colorPixels;
    
};
