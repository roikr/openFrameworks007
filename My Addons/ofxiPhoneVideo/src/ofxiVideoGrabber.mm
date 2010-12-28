/*
 *  ofxiVideoGrabber.cpp
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiVideoGrabber.h"

#import "MyVideoBuffer.h"
#include <iostream>
#include "ofMain.h"
#include "ofxiPhoneVideo.h"
#import "glu.h"


ofxiVideoGrabber::ofxiVideoGrabber() {
	state = CAMERA_IDLE;
}

int ofxiVideoGrabber::getState() {
	return state;
}
	

void ofxiVideoGrabber::setup(ofxiPhoneVideo *video) {
	
	
	this->video = video;
	
	videoTexture = [[[MyVideoBuffer alloc] initWithFPS:video->fps] retain];
	
	
	glGenFramebuffersOES(1, &fbo);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);
	
	
		
	for (int i=0; i<video->numFrames; i++) {
	
		GLuint texture;
		
		 
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, video->textureWidth, video->textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, NULL);     // check if this is right
		
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, texture, 0); // probably for init and alloc mem
		
		glBindTexture(GL_TEXTURE_2D,0);
		
		GLuint status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
		if (status != GL_FRAMEBUFFER_COMPLETE_OES)
		{
			NSLog(@"failed to make complete framebuffer object %x", status);
		}
		
		
		video->textures.push_back(texture);
				
	}
	
}

void ofxiVideoGrabber::fboDraw()
{
	if ((state == CAMERA_RECORDING || state == CAMERA_CAPTURING)  ) {
		
		if (cameraFrame == videoTexture.currentFrame) {
			//NSLog(@"do nothing");
			return;
		}
		
		cameraFrame++;
		
		if (videoTexture.currentFrame - cameraFrame > 0) {
			NSLog(@"skip %i frames",videoTexture.currentFrame - cameraFrame );
		}
		
		GLuint texture = video->textures[currentFrame % video->numFrames];
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, fbo);
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, texture, 0);
		glBindTexture(GL_TEXTURE_2D,0);
		
		
		glViewport(0, 0, video->textureWidth, video->textureHeight);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity ();
		gluOrtho2D (0, video->textureWidth, 0, video->textureHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
				
		glColor4f(1.0, 1.0, 1.0, 0);
		
		
		[videoTexture renderCameraToSprite:videoTexture.CameraTexture withWidth:480];
			
		currentFrame++;
		
		if (state == CAMERA_RECORDING) {
			if ( currentFrame-1 - firstFrame >= video->numFrames - video->numIntroFrames)  {
				state = CAMERA_RUNNING;
			}
		}
	}
}




void ofxiVideoGrabber::draw() {

	
	//[videoTexture renderCameraToSprite:videoTexture.CameraTexture withWidth:480];
	
	if (state == CAMERA_CAPTURING || state == CAMERA_RECORDING) {
		GLuint texture = video->textures[(currentFrame-1) % video->numFrames];
		video->drawTexture(texture);
		
		
		
	}
	
//	if (video->bMirrored) {
//		glTranslatef(video->width, 0, 0);
//		glScalef(-1.0, 1.0, 1.0);
//	}
	

}




	

void ofxiVideoGrabber::exit() {
	stopCamera();
	[videoTexture release];
	videoTexture = nil;
	
}

void ofxiVideoGrabber::audioReceived( float * input, int bufferSize) {
	if( video->audio.getBufferSize() != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoGrabber: your buffer size was set to %i - but the stream needs a buffer size of %i", video->audio.getBufferSize(), bufferSize);
		return;
	}	
	
	if (video->audio.getIsRecording()) {
		video->audio.audioReceived(input,bufferSize);
		if (!video->audio.getIsRecording()) {
			state = CAMERA_RUNNING;
			video->audio.normalize();
		}
	}
	
//	if (getState()==CAMERA_RECORDING) {
//		
//		for (int i = 0; i < bufferSize; i++){
//			video->sample[video->audio.getBufferSize()*currentBuffer+i] = input[i];
//		}
//		currentBuffer++;
//		
//		if (currentBuffer>=video->numBuffers) {
//			state = CAMERA_RUNNING;
//		}
//	}
	
	
	//scaledSamples.push_front(max);
	
	
}




void ofxiVideoGrabber::startCamera() {
	
	if (getState()!=CAMERA_IDLE)
		return;
	
	[videoTexture._session startRunning];
	state = CAMERA_RUNNING;
	
}

void ofxiVideoGrabber::stopCamera() {
	
	if (getState()==CAMERA_IDLE)
		return;
	
	if([videoTexture._session isRunning])
		[videoTexture._session stopRunning];
	
	state = CAMERA_IDLE;
}


void ofxiVideoGrabber::startCapture() {
	state = CAMERA_CAPTURING;
	
	currentFrame = 0;
	
	cameraFrame = videoTexture.currentFrame-1;
}

void ofxiVideoGrabber::record() {
	
	video->audio.record();
		
	firstFrame = currentFrame-1;
	video->firstFrame = (currentFrame-1) % video->numFrames;
	
	state = CAMERA_RECORDING;
	
	ofLog(OF_LOG_VERBOSE, "ofxiVideoGrabber::record, first frame: %i", firstFrame);
	
}

bool ofxiVideoGrabber::cameraToggle() {
	return [videoTexture cameraToggle] == YES;
}










