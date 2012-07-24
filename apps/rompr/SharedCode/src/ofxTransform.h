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
    void setup(ofVec2f pos=ofVec2f(0,0),float scl=1.0f,float rot=0.0f);
    void start(int ease_function,ofVec2f pos=ofVec2f(0,0),float scl=1.0f,float rot=0.0f);
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