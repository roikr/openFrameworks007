/*
 *  ofxiOffScreen.cpp
 *  iPhoneCameraExample
 *
 *  Created by Roee Kremer on 11/1/12.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiOffScreen.h"
#include "glu.h"
#include "ofxiPhoneExtras.h"


void ofxiOffScreen::setup(int width,int height) {
	this->width = width;
	this->height = height;
	glGenFramebuffersOES(1, &framebuffer);
    glGenRenderbuffersOES(1, &colorRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_RGBA8_OES, width, height);
    
    
   
   	
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER_OES) ;
    if(status != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", status);
    }
}


void ofxiOffScreen::begin() {
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &defaultFramebuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D (0, width, height , 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glScalef(1.0, -1.0,1.0);
	glTranslatef(0, -height, 0);
	
	glColor4f(1.0, 1.0, 1.0, 0);
}




void ofxiOffScreen::end() {
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
	
}


void ofxiOffScreen::exit() {
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	glDeleteFramebuffersOES(1, &framebuffer);
	NSLog(@"ofxiOffScreen::exit - glDeleteFramebuffersOES: %i", framebuffer);
	framebuffer = 0;
    glDeleteRenderbuffersOES(1, &colorRenderbuffer);
    
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
	
	
}
 
