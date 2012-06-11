//
//  ofxClouds.h
//  shaderExample
//
//  Created by Roee Kremer on 3/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef shaderExample_ofxClouds_h
#define shaderExample_ofxClouds_h

#include "ofMain.h"


class ofxClouds {
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    
    
    
    
    
private:
    
    ofShader shader;
    
    ofTexture fbm;
    float maxAlpha;
    float velocity;
};

#endif
