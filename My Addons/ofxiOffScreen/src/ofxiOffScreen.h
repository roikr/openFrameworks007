/*
 *  ofxiOffScreen.j
 *  iPhoneCameraExample
 *
 *  Created by Roee Kremer on 11/1/12.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


class ofxiOffScreen {
public:
	
	void setup(int width,int height);
	void exit();
	void begin();
	void end();
	
	
	GLuint framebuffer;
    GLuint colorRenderbuffer;
    
	int width;
	int height;
    
    GLint defaultFramebuffer;
};