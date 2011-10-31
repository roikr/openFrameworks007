//
//  ofxTexturesVector.cpp
//  MixMeXmas
//
//  Created by Roee Kremer on 10/31/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxTexturesVector.h"
#include "ofxRKTexture.h"
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include "ofMain.h"
#include <iostream>
#include "ofMainExt.h"

#define LOG_TEXTURES_VECTOR

void ofxTexturesVector::setup(string filename, int numTextures ) {
	
	
	this->filename = filename;
	
	bInitialized = false;
    
    for (int i=0; i<numTextures; i++) {
        ofxRKTexture *texture = new ofxRKTexture;
        texture->setup(filename+"_"+ofxIntToString(i, 5)+".pvr");
        textures.push_back(texture);
    }
}

void ofxTexturesVector::init() {
    for (vector<ofxRKTexture*>::iterator iter=textures.begin(); iter!=textures.end(); iter++) {
        (*iter)->init();
    }
    bInitialized = true;
    titer = textures.begin();
}

void ofxTexturesVector::release() {
    for (vector<ofxRKTexture*>::iterator iter=textures.begin(); iter!=textures.end(); iter++) {
        (*iter)->release();
    }
    bInitialized = false;
}

void ofxTexturesVector::setTexture(int i) {
   titer=textures.begin()+i;
}

void ofxTexturesVector::draw() {
    if (bInitialized) {
        (*titer)->draw(0,0);
    }
}

