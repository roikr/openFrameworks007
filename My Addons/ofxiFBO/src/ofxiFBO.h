/*
 *  ofxiFBO.h
 *  iPhoneCameraExample
 *
 *  Created by Roee Kremer on 12/30/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

class ofxiFBO {
public:
	
	void setup();
	void push(int width,int height);
	void begin(GLuint texture);
	void end();
	void pop();
	
	GLuint fbo;
	GLuint oldFramebuffer;
	
};