//
//  ofxFBO.h
//  dctExample
//
//  Created by Roee Kremer on 2/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"

class ofxFBO {
	
public:
    void setup(int width, int height);
    void draw();
    void exit();
    
    void begin();
    void end();
    
    GLuint fboName;
    GLuint colorTexture;
    
    int width;
    int height;
};