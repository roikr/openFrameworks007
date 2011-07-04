/*
 *  ofxiTexture.h
 *  iPhonePincherExample
 *
 *  Created by Roee Kremer on 1/2/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include <string>

using namespace std;

enum {
	OFX_TEXTURE_TYPE_PVR,
	OFX_TEXTURE_TYPE_PNG
};

class ofxiTexture {

	public:
	
	bool load(string filename,int type=OFX_TEXTURE_TYPE_PVR);
	void draw();
	void release();
	
	GLuint _name;
	uint32_t _width, _height;
	GLenum _internalFormat;
	bool _hasAlpha;


};