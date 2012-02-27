//
//  ofxFBO.cpp
//  dctExample
//
//  Created by Roee Kremer on 2/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxFBO.h"

#include "glUtil.h"
#define GetGLError()									\
{														\
GLenum err = glGetError();							\
while (err != GL_NO_ERROR) {						\
printf("GLError %s set in File:%s Line:%d\n",	\
GetGLErrorString(err),					\
__FILE__,								\
__LINE__);								\
err = glGetError();								\
}													\
}


void deleteFBOAttachment(GLenum attachment)
{    
    GLint param;
    GLuint objName;
	
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment,
                                          GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                          &param);
	
    if(GL_RENDERBUFFER == param)
    {
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                              &param);
		
        objName = ((GLuint*)(&param))[0];
        glDeleteRenderbuffers(1, &objName);
    }
    else if(GL_TEXTURE == param)
    {
        
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                              &param);
		
        objName = ((GLuint*)(&param))[0];
        glDeleteTextures(1, &objName);
    }
    
}

void destroyFBO(GLuint fboName) 
{ 
	if(0 == fboName)
	{
		return;
	}
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboName);
	
	
    GLint maxColorAttachments = 1;
	
	
	// OpenGL ES on iOS 4 has only 1 attachment. 
	// There are many possible attachments on OpenGL 
	// on MacOSX so we query how many below
#if !ESSENTIAL_GL_PRACTICES_IOS
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
#endif
	
	GLint colorAttachment;
	
	// For every color buffer attached
    for(colorAttachment = 0; colorAttachment < maxColorAttachments; colorAttachment++)
    {
		// Delete the attachment
		deleteFBOAttachment(GL_COLOR_ATTACHMENT0+colorAttachment);
	}
	
	// Delete any depth or stencil buffer attached
    deleteFBOAttachment(GL_DEPTH_ATTACHMENT);
	
    deleteFBOAttachment(GL_STENCIL_ATTACHMENT);
	
    glDeleteFramebuffers(1,&fboName);
}

void ofxFBO::setup(int width, int height) {
    
   
    glGenFramebuffers(1, &fboName);
	glBindFramebuffer(GL_FRAMEBUFFER, fboName);	
	
    
    this->width = width;
    this->height = height;
	
	// Create a texture object to apply to model
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	
	// Set up filter and wrap modes for this texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if ESSENTIAL_GL_PRACTICES_IOS
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR_MIPMAP_LINEAR
#endif
	
	// Allocate a texture image with which we can render to
	// Pass NULL for the data parameter since we don't need to load image data.
	//     We will be generating the image by rendering to this texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
				 width, height, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
//	GLuint depthRenderbuffer;
//	glGenRenderbuffers(1, &depthRenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	
	
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
	
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		destroyFBO(fboName);
		return 0;
	}
	
	GetGLError();
    
    
//	glBindFramebuffer(GL_FRAMEBUFFER,fboName);
//    GLint texName;
//    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,&texName);
//    fboTexture = ((GLuint*)(&texName))[0];
    

	
}

void ofxFBO::draw() {
    
}

void ofxFBO::exit() {
    
}

void ofxFBO::begin() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboName);
	
	
	glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
}

void ofxFBO::end() {
    // Bind our default FBO to render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
	glViewport(0, 0, ofGetWidth(), ofGetHeight());
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}