/*
 *  ofxFBOTexture.cpp
 *  openFrameworks
 *
 *  Created by Zach Gage on 3/28/08.
 *  Copyright 2008 STFJ.NET. All rights reserved.
 *
 *  http://addons.openframeworks.cc/projects/show/ofxfbotexture
 *
 *  Updated 22nd July 2009
 *
 */

#include "ofxFBOTexture.h"
#include "glu.h"

#define GLEE_ARB_texture_rectangle false

void ofxFBOTexture::allocate(int w, int h, bool autoClear) {
        _isActive = false;

        texData.width = w;
        texData.height = h;

//    if (GLEE_ARB_texture_rectangle){
//        texData.tex_w = w;
//        texData.tex_h = h;
//        //texData.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
//    } else {
        texData.tex_w = ofNextPow2(w);
        texData.tex_h = ofNextPow2(h);
//    }

//        if (GLEE_ARB_texture_rectangle){
//                texData.tex_t = w;
//                texData.tex_u = h;
//        } else {
                texData.tex_t = w/texData.tex_w;
                texData.tex_u = h/texData.tex_h;
//        }

        // attempt to free the previous bound texture, if we can:
        clean();

        texData.width = w;
        texData.height = h;
        texData.bFlipTexture = true;
        texData.glType = GL_RGBA;

        this->autoClear = autoClear;
        // create & setup FBO
				
				
		//glGetIntegerv(GL_RENDERBUFFER_BINDING_OES, (GLint *) &oldRenderbuffer);
		glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, (GLint *) &oldFramebuffer);
			
        glGenFramebuffersOES(1, &fbo);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);

//        // Create the render buffer for depth
//        //glGenRenderbuffersOES(1, &depthBuffer);
//        //glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthBuffer);
//        //glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT, texData.tex_w, texData.tex_h);
//
//        // create & setup texture

        glGenTextures(1, (GLuint *)texData.textureName);   // could be more then one, but for now, just one
        glBindTexture(texData.textureTarget, (GLuint)texData.textureName[0]);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(texData.textureTarget, 0, texData.glType, texData.tex_w, texData.tex_h, 0, texData.glType, GL_UNSIGNED_BYTE, 0);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			
//
//        // attach it to the FBO so we can render to it
        glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, texData.textureTarget, (GLuint)texData.textureName[0], 0);


        // Attach the depth render buffer to the FBO as it's depth attachment
        // glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthBuffer);


//        GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
//        if(status != GL_FRAMEBUFFER_COMPLETE_OES) {
//                cout<<"glBufferTexture failed to initialize. Perhaps your graphics card doesnt support the framebuffer extension? If you are running osx prior to system 10.5, that could be the cause"<<endl;
//                std::exit(1);
//        }
        //clear(0, 0, 0, 0);

		//glBindRenderbufferOES(GL_RENDERBUFFER_OES, oldRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, oldFramebuffer);

        texData.bAllocated = true;

}

void ofxFBOTexture::setupScreenForMe(){

        int w = texData.width;
        int h = texData.height;

        float halfFov, theTan, screenFov, aspect;
        screenFov               = 60.0f;

        float eyeX              = (float)w / 2.0;
        float eyeY              = (float)h / 2.0;
        halfFov                 = PI * screenFov / 360.0;
        theTan                  = tanf(halfFov);
        float dist              = eyeY / theTan;
        float nearDist  = dist / 10.0;  // near / far clip plane
        float farDist   = dist * 10.0;
        aspect                  = (float)w/(float)h;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(screenFov, aspect, nearDist, farDist);
        gluLookAt(eyeX, eyeY, dist, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
        glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.

    glViewport(0,0,texData.width, texData.height);

}

void ofxFBOTexture::setupScreenForThem(){
//      glPopAttrib();
//      return;

    int w, h;

        w = ofGetWidth();
        h = ofGetHeight();

        float halfFov, theTan, screenFov, aspect;
        screenFov               = 60.0f;

        float eyeX              = (float)w / 2.0;
        float eyeY              = (float)h / 2.0;
        halfFov                 = PI * screenFov / 360.0;
        theTan                  = tanf(halfFov);
        float dist              = eyeY / theTan;
        float nearDist  = dist / 10.0;  // near / far clip plane
        float farDist   = dist * 10.0;
        aspect                  = (float)w/(float)h;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(screenFov, aspect, nearDist, farDist);
        gluLookAt(eyeX, eyeY, dist, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
        glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.


    glViewport(0,0,w, h);
}

void ofxFBOTexture::swapIn() {
        _isActive = true;

		glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, (GLint *) &oldFramebuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo); // bind the FBO to the screen so we can draw to it

        if(autoClear) clear();
}

void ofxFBOTexture::swapOut() {
        _isActive = false;

//        glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0); //unbind the FBO
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, oldFramebuffer);
		
}

void ofxFBOTexture::clean()
{
        // try to free up the texture memory so we don't reallocate
        // http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/deletetextures.html
        if (texData.textureName[0] != 0){
				glDeleteFramebuffersOES(1, &fbo);
      			//glDeleteRenderbuffersOES(1, &depthBuffer);
                glDeleteTextures(1, (GLuint *)texData.textureName);
        }
        texData.width = 0;
        texData.height = 0;
        texData.bFlipTexture = false;
}

void ofxFBOTexture::clear() {
        bool alreadyIn = _isActive;
        if(!alreadyIn) begin();         // MEMO
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT );     // Clear Screen data of the texture to write on
        if(!alreadyIn) end();           // MEMO
}

void ofxFBOTexture::clear(float r, float g, float b, float a) {
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
    clear();
}

void ofxFBOTexture::bindAsTexture(){
	glBindTexture(GL_TEXTURE_2D, (GLuint)texData.textureName);
}
