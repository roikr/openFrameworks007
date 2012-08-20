/*
 *  ofxiFbo.h
 *  iPhoneCameraExample
 *
 *  Created by Roee Kremer on 12/30/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


class ofxiFbo {
public:
	
	void setup(int width,int height);
	void exit();
	void begin(GLuint texture);
	void end();
	
	
	GLuint fbo;
	int width;
	int height;
};