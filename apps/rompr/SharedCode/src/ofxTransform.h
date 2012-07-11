//
//  ofxTransform.h
//  Rompr
//
//  Created by Roee Kremer on 7/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofxPenner.h"

class ofxTransform {
public:
    void start(int ease_function,ofVec2f pos1,ofVec2f pos2,float scl1=1.0f,float scl2=1.0f,float rot1=0.0f,float rot2=0.0f);
    void update();
    bool getIsEasing();
    void begin();
    void end();
        
private:
    ofxPenner penner;
    
    ofVec2f pos;
    float scl;
    float rot;
};