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

class ofxiTexture {

	public:
	
	bool load(string filename);
	void draw();
	
	GLuint _name;
	uint32_t _width, _height;
	GLenum _internalFormat;
	bool _hasAlpha;


};