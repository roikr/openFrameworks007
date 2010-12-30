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
#include "ofxiPhoneExtras.h"


ofxiVideoGrabber::ofxiVideoGrabber() {
	state = CAMERA_IDLE;
}

int ofxiVideoGrabber::getState() {
	return state;
}
	

void ofxiVideoGrabber::setup(ofxiPhoneVideo *video) {
	
	
	this->video = video;
	
	videoTexture = [[[MyVideoBuffer alloc] initWithFPS:video->fps] retain];
	
	fbo.setup();
	
	
		
	for (int i=0; i<video->numFrames; i++) {
	
		GLuint texture;
		
		 
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, video->textureWidth, video->textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, NULL);     // check if this is right
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
		fbo.begin(texture,video->textureWidth,video->textureHeight);
		fbo.end();
		
		video->textures.push_back(texture);
				
	}
}





void ofxiVideoGrabber::drawLiveCam() {
	
	
	float w = 480; // normalizing each camera to 480 * 360
	float h = (float)videoTexture.videoDimensions.height / (float)videoTexture.videoDimensions.width * w ;
	
	
	glEnable(GL_TEXTURE_2D);
	
	// bind the texture
	glBindTexture(GL_TEXTURE_2D, videoTexture.CameraTexture );
	
	GLfloat px0 = 0;		// up to you to get the aspect ratio right
	GLfloat py0 = 0;
	GLfloat px1 = w;
	GLfloat py1 = h;
	
			
	
	GLfloat tx0 = 0;
	GLfloat ty0 = 0;
	GLfloat tx1 = 1;
	GLfloat ty1 = 1;
	
	glPushMatrix(); 
	glScalef(0.5, 0.5, 0);
	//glTranslatef(x,y,0.0f);
	
	GLfloat tex_coords[] = {
		tx0,ty0,
		tx1,ty0,
		tx1,ty1,
		tx0,ty1
	};
	GLfloat verts[] = {
		px0,py0,
		px1,py0,
		px1,py1,
		px0,py1
	};
	
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	
	
}

void ofxiVideoGrabber::render() {
	
	if ((state == CAMERA_RECORDING || state == CAMERA_CAPTURING)) {
		
		if  (cameraFrame != videoTexture.currentFrame ) {
		
			cameraFrame++;
			
			if (videoTexture.currentFrame - cameraFrame > 0) {
				NSLog(@"skip %i frames",videoTexture.currentFrame - cameraFrame );
			}
			
			GLuint texture = video->textures[currentFrame % video->numFrames];
			fbo.begin(texture,video->textureWidth,video->textureHeight);		
			
			[videoTexture renderCameraToSprite:videoTexture.CameraTexture withWidth:480];
			
			currentFrame++;
			
			if (state == CAMERA_RECORDING) {
				if ( currentFrame-1 - firstFrame >= video->numFrames - video->numIntroFrames)  {
					state = CAMERA_RUNNING;
				}
			}
			
			
			fbo.end();
		}
	}
	

}

void ofxiVideoGrabber::draw() {
	
	if ((state == CAMERA_RECORDING || state == CAMERA_CAPTURING)) {
		
		video->drawTexture(video->textures[(currentFrame-1) % video->numFrames]);
	} else {
		//[videoTexture renderCameraToSprite:videoTexture.CameraTexture withWidth:480];
		drawLiveCam();
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










