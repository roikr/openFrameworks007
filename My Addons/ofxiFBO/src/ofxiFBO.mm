/*
 *  ofxiFBO.cpp
 *  iPhoneCameraExample
 *
 *  Created by Roee Kremer on 12/30/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiFBO.h"
#include "glu.h"
#include "ofxiPhoneExtras.h"


void ofxiFBO::setup(int width,int height) {
	this->width = width;
	this->height = height;
	glGenFramebuffersOES(1, &fbo);
	NSLog(@"ofxiFBO::setup - glGenFramebuffersOES: %i", fbo);
	
}

void ofxiFBO::begin(GLuint texture) {
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);
	glBindTexture(GL_TEXTURE_2D, texture);
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, texture, 0); // probably for init and alloc mem
	
	glBindTexture(GL_TEXTURE_2D,0);
	
	GLuint status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	if (status != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"ofxiFBO::begin - failed to make complete framebuffer object %x, for texture: %i", status,texture);
		switch (status) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES:
				NSLog(@"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES:
				NSLog(@"GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES:
				NSLog(@"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED_OES:
				NSLog(@"GL_FRAMEBUFFER_UNSUPPORTED_OES");
				break;
			default:
				break;
		}
	}
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D (0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glScalef(1.0, -1.0,1.0);
	glTranslatef(0, -height, 0);
	
	glColor4f(1.0, 1.0, 1.0, 0);
}




void ofxiFBO::end() {
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
	
}


void ofxiFBO::exit() {
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, 0, 0); 
	glDeleteFramebuffersOES(1, &fbo);
	NSLog(@"ofxiFBO::exit - glDeleteFramebuffersOES: %i", fbo);
	fbo = 0;
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
	
	
}
