//
//  ofxiExternalView.cpp
//  externalDisplay
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiExternalView.h"
#include "ofxiPhoneExtras.h"

vector<pair<int,int> > ofxiExternalView::listModes() {
    NSArray	*screens = [UIScreen screens];
    vector<pair<int,int> > modes;
    
    if ([screens count] > 1) {
	
        NSArray *displayModes = [[screens objectAtIndex:1] availableModes];
        for (UIScreenMode	*mode in displayModes) {
            modes.push_back(make_pair(mode.size.width, mode.size.height));
            
        }
            
            
        
    }
    return modes;
}


void ofxiExternalView::setupMode(int mode) {
    
    NSArray	*screens = [UIScreen screens];	
	NSUInteger screenCount = [screens count];
	
    
    
	if (screenCount > 1) {
		// 2.
		
		// Select first external screen
		extScreen = [screens objectAtIndex:1];
        
        if (mode<[[extScreen availableModes] count]) {
        extScreen.currentMode = [[extScreen availableModes] objectAtIndex:mode];
        
            if (extWindow == nil || !CGRectEqualToRect(extWindow.bounds, [extScreen bounds])) {
                // Size of window has actually changed
                
                // 4.
                extWindow = [[UIWindow alloc] initWithFrame:[extScreen bounds]];
                
                // 5.
                extWindow.screen = extScreen;

                
                glView = [[ExternalGLView alloc] initWithFrame:[extWindow frame] andDepth:iPhoneGetOFWindow()->isDepthEnabled() andContext:[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:[iPhoneGetGLView().context sharegroup]]];
                
       
                
                [extWindow addSubview:glView];
                //            [view release];
                
                // 6.
                
                // 7.
                [extWindow makeKeyAndVisible];
                
                bExtSelected = true;
                
            }
        }
    }
  
}



void ofxiExternalView::begin() {
    if (bExtSelected) {
        CGSize size = [extWindow frame].size;
        [glView startRender];
        
        //we do this as ofGetWidth() now accounts for rotation 
        //so we just make our viewport across the whole screen
        glViewport( 0, 0, size.width, size.height );
        
        float * bgPtr = ofBgColorPtr();
        bool bClearAuto = ofbClearBg();
        if ( bClearAuto == true){
            glClearColor(bgPtr[0],bgPtr[1],bgPtr[2], bgPtr[3]);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        
        ofSetupScreenPerspective(size.width, size.height, ofGetOrientation(), true);
        
        
    }
}

void ofxiExternalView::end() {
    if (bExtSelected) {
        [glView finishRender];
    }
}

void ofxiExternalView::exit() {
    
}

bool ofxiExternalView::getIsActive() {
    return bExtSelected;
}