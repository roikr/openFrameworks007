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
#include "ofMainExt.h"

void ofxiFBO::setup() {
	glGenFramebuffersOES(1, &fbo);
}

void ofxiFBO::push(int width,int height) {
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity ();
	gluOrtho2D (0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glColor4f(1.0, 1.0, 1.0, 0);
}

void ofxiFBO::begin(GLuint texture) {
	
	
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, (GLint *) &oldFramebuffer); 
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);
	glBindTexture(GL_TEXTURE_2D, texture);
	glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, texture, 0); // probably for init and alloc mem
	
	glBindTexture(GL_TEXTURE_2D,0);
	
	GLuint status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
	if (status != GL_FRAMEBUFFER_COMPLETE_OES)
	{
		NSLog(@"failed to make complete framebuffer object %x", status);
	}
	
	
	
	
}



void ofxiFBO::end() {
	
	
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, oldFramebuffer);
	
	}


void ofxiFBO::pop() {
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	ofxiPhoneGLViewPort();

}
