//
//  ofxiStill.h
//  stillExample
//
//  Created by Roee Kremer on 7/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once


#include "ofMain.h"
#import <CoreVideo/CVOpenGLESTextureCache.h>

extern ofEvent<ofImage> ofxiStillCameraEvent;

template<class T>
void ofxRegisterStillCameraNotification(T * obj){
	ofAddListener(ofxiStillCameraEvent,obj,&T::pictureTaken);
}

template<class T>
void ofxUnregisterStillCameraNotification(T * obj){
	ofRemoveListener(ofxiStillCameraEvent,obj,&T::pictureTaken);
}

@class StillCamera;

class ofxiStill {
	
public:
	
	void setup();
	void update();
	void draw(ofRectangle rect,ofRectangle tex);
	
    void preview();
    void snap();
    void stop();
    
	
    float getFrameRate();
	
	float getWidth();
	float getHeight();
    
	bool getIsPlaying();
    bool getIsFrameVisible();
    
    void bind();
    void unbind();
    
//    ofImage image;
    bool bSnap;
	
protected:
    void cleanUpTextures();
    
	StillCamera *stillCamera;
    
    CVOpenGLESTextureCacheRef videoTextureCache; 
    CVOpenGLESTextureRef videoTexture;
    
    bool bPlaying;
    
    int width;
    int height;
    
    
};

