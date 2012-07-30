//
//  ExternalGLView.m
//  emptyExample
//
//  Created by Roee Kremer on 7/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//



/***********************************************************************
 
 Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * ***********************************************************************/ 


#import "ofMain.h"
#import  "ofxiPhoneExtras.h"

#import "ExternalGLView.h"

#import "ES1Renderer.h"
#import "ES2Renderer.h"

@implementation ExternalGLView

// You must implement this method
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (id) initWithFrame:(CGRect)frame
{
	return [self initWithFrame:frame andDepth:false andContext:[ofxiPhoneGetGLView() context]];
}

- (id) initWithFrame:(CGRect)frame andDepth:(bool)depth andContext:(EAGLContext*) theContext
{
	if((self = [super initWithFrame:frame])) {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)super.layer;
		
        eaglLayer.opaque = true;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
				
		
		
        depthEnabled = depth;
				
        
        context = theContext;
		NSLog(@"Using OpenGL ES1 Renderer");
        
        if (!context || ![EAGLContext setCurrentContext:context])
        {
			NSLog(@"OpenGL ES1 failed");
            [self release];
            return nil;
        }
        
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &defaultFramebuffer);
        glGenRenderbuffersOES(1, &colorRenderbuffer);
		
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
		
				
		[[self context] renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer];
		
		
		self.multipleTouchEnabled = false;
		self.opaque = true;
        
//        [self layoutSubviews];
		
	}
	
	return self;
}

- (void)startRender
{
    [EAGLContext setCurrentContext:context];
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
	
    // This application only creates a single default framebuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple framebuffers.
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
}

- (void)finishRender
{
    // This application only creates a single color renderbuffer which is already bound at this point.
    // This call is redundant, but needed if dealing with multiple renderbuffers.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
	
	
	
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
	
}


- (void)layoutSubviews
{
    NSLog(@"layoutSubviews");
    //	layer.opaque = YES;
    // Allocate color buffer backing based on the current layer size
    //[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
	
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
		
	if(depthEnabled)
	{
		if(!depthRenderbuffer)
			glGenRenderbuffersOES(1, &depthRenderbuffer);
		
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		
    
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight); // GL DEPTH COMPONENT ON THIS LINE ISNT CORRECT POTENTIALLY
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
		
        
	}
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
	
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
    {
        NSLog(@"Failed to make complete framebuffer object %x %ix%i", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES), backingWidth, backingHeight);
        
    }
    
    
    [self startRender];
    [self finishRender];
}


- (void) dealloc
{
    // Tear down GL
    if (defaultFramebuffer)
    {
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
    
    if (colorRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
    
	if(depthRenderbuffer) {
		glDeleteRenderbuffersOES(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
	
	
    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    

	[super dealloc];
}

-(EAGLContext*) context
{
	return context;
}

@end
