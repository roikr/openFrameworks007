//
//  ofxiExternalView.h
//  externalDisplay
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ExternalGLView.h"

class ofxiExternalView {
public:
    ofxiExternalView():bExtSelected(false) {};
    vector<pair<int,int> > listModes();
    void setupMode(int mode);
    void begin();
    void end();
    void exit();
    bool getIsActive();
    
    
private:
    
    ExternalGLView *glView;
    UIScreen					*extScreen;
	UIWindow					*extWindow;
    
    
    
    bool bExtSelected;

    
};