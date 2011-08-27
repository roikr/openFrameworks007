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

#import "glu.h"
#include "ofxiPhoneExtras.h"
#include "ofxiPhonevideo.h"


ofxiVideoGrabber::ofxiVideoGrabber() {
	state = CAMERA_IDLE;
}

int ofxiVideoGrabber::getState() {
	return state;
}
	

void ofxiVideoGrabber::setup(ofxiPhoneVideo *video,int cameraPosition) {
	
	videoTexture = [[[MyVideoBuffer alloc] initWithFPS:video->fps devicePosition:cameraPosition == FRONT_CAMERA ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack] retain];
	
	
	
//	fbo.setup();
	
	fbo.setup(video->textureWidth, video->textureHeight);
	
	this->video = video;
}

void ofxiVideoGrabber::initVideo() {
	
	for (int i=0; i<video->numFrames; i++) {
		
		GLuint texture;
		
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, video->textureWidth, video->textureHeight, 0,  GL_RGBA, GL_UNSIGNED_BYTE, NULL);     // check if this is right
		
		glBindTexture(GL_TEXTURE_2D, 0);
		
		printf("creating texture: %i\n",texture);
		
		//		fbo.begin(texture);
		//		fbo.end();
		
		video->textures.push_back(texture);
		
	}
}


void ofxiVideoGrabber::releaseVideo() {
	
	for (vector<int>::iterator iter=video->textures.begin(); iter!=video->textures.end(); iter++) {
		GLuint texture = *iter;
		glDeleteTextures(1, &texture);
	}
	video->textures.clear();
}


//void ofxiVideoGrabber::setCameraPosition(int cameraPosition) {
//	
//}


void ofxiVideoGrabber::drawFullCamera() {
	

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
	GLfloat ty0 = 1;
	GLfloat tx1 = 1;
	GLfloat ty1 = 0;
	
	if (videoTexture.devicePosition == AVCaptureDevicePositionBack) {
		tx0 = 0;
		ty0 = 0;
		tx1 = 1;
		ty1 = 1;
	}
	
	
//	glScalef(0.5, 0.5, 0);
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
	
	
	glBindTexture(GL_TEXTURE_2D, 0 );
	glDisable(GL_TEXTURE_2D);
	
	
}

void ofxiVideoGrabber::drawCamera() {
	
	if (!getIsReady()) {
		return;
	}
	
	
	
	
	glEnable(GL_TEXTURE_2D);
	
	// bind the texture
	glBindTexture(GL_TEXTURE_2D, videoTexture.CameraTexture );
	
	GLfloat px0 = 0;		// up to you to get the aspect ratio right
	GLfloat py0 = 0;
	GLfloat px1 = video->textureWidth;
	GLfloat py1 = video->textureHeight;
	
	
	GLfloat tx0 = rect.x;
	GLfloat ty0 = rect.y+rect.height;
	GLfloat tx1 = rect.x+rect.width;
	GLfloat ty1 = rect.y;
	
	if (videoTexture.devicePosition == AVCaptureDevicePositionBack) {
		ty0 = rect.y;
		ty1 = rect.y+rect.height;
	}
	
	 
	//	glScalef(0.5, 0.5, 0);
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
	
	
	
	glBindTexture(GL_TEXTURE_2D, 0 );
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
			fbo.begin(texture);	
			
			drawCamera();
			
			currentFrame++;
			
			if (state == CAMERA_RECORDING) {
				if ( currentFrame-1 - firstFrame >= video->numFrames - video->numIntroFrames-1)  {
					state = CAMERA_RUNNING;
				}
			}
			
			
			fbo.end();
		}
	}
}



void ofxiVideoGrabber::draw() {
	
	if ((state == CAMERA_RECORDING || state == CAMERA_CAPTURING)) {
		
		drawTexture(video->textures[(currentFrame-1) % video->numFrames]);
	} else {
		drawCamera();
	}
	
	
	//	if (video->bMirrored) {
	//		glTranslatef(video->width, 0, 0);
	//		glScalef(-1.0, 1.0, 1.0);
	//	}
	
	
}


void ofxiVideoGrabber::drawTexture(int texture) {
	
	
	if (!video->bHorizontal) {
		ofTranslate(video->textureHeight, 0, 0);
		ofRotate(90);
		
	}
	
	float u = video->widthFraction;
	float v = video->heightFraction;
	
	GLfloat spriteTexcoords[] = {
		u,v,   
		u,0.0f,
		0,v,   
		0.0f,0,};
	
	float w = video->textureWidth*u;//ofGetWidth()/2;
	float h = video->textureHeight*v;//(float)ofGetWidth()/(float)width*(float)height/2;
	
	GLfloat spriteVertices[] =  {
		w,h,0,   
		w,0,0,   
		0,h,0, 
		0,0,0};
	
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	//	glPushMatrix();
	//	//	if (video->bMirrored) {
	//	//		glTranslatef(w, 0, 0);
	//	//		glScalef(-1.0, 1.0, 1.0);
	//	//	}
	//	
	//	glPopMatrix();

	
}



	

void ofxiVideoGrabber::exit() {
	stopCamera();
	[videoTexture release];
	videoTexture = nil;
	
}

//void ofxiVideoGrabber::audioReceived( float * input, int bufferSize) {
//	if( video->audio.getBufferSize() != bufferSize ){
//		ofLog(OF_LOG_ERROR, "ofxiVideoGrabber: your buffer size was set to %i - but the stream needs a buffer size of %i", video->audio.getBufferSize(), bufferSize);
//		return;
//	}	
//	
//	if (video->audio.getIsRecording()) {
//		video->audio.audioReceived(input,bufferSize);
//		if (!video->audio.getIsRecording()) {
//			grabber.stop();
//			video->audio.normalize();
//		}
//	}
//	
////	if (getState()==CAMERA_RECORDING) {
////		
////		for (int i = 0; i < bufferSize; i++){
////			video->sample[video->audio.getBufferSize()*currentBuffer+i] = input[i];
////		}
////		currentBuffer++;
////		
////		if (currentBuffer>=video->numBuffers) {
////			state = CAMERA_RUNNING;
////		}
////	}
//	
//	
//	//scaledSamples.push_front(max);
//	
//	
//}




void ofxiVideoGrabber::startCamera() {
	
	if (getState()!=CAMERA_IDLE)
		return;
	
//	fbo.setup();
	
//	for (vector<int>::iterator tex=video->textures.begin(); tex!=video->textures.end(); tex++) {
//		fbo.begin(*tex);
//		fbo.end();
//
//	}
	
	[videoTexture._session startRunning];
	state = CAMERA_RUNNING;
	
}

void ofxiVideoGrabber::stopCamera() {
	
	if (getState()==CAMERA_IDLE)
		return;
	
	if([videoTexture._session isRunning])
		[videoTexture._session stopRunning];
	
	state = CAMERA_IDLE;
//	fbo.release();
}


void ofxiVideoGrabber::startCapture() {
	if (video->textures.empty()) {
		initVideo();
	}
	
	state = CAMERA_CAPTURING;
	
	currentFrame = 0;
	
	cameraFrame = videoTexture.currentFrame-1;
}

void ofxiVideoGrabber::record() {
// roikr: audio fix	
//	video->audio.record();
		
	firstFrame = currentFrame-1;
	video->firstFrame = (currentFrame-1) % video->numFrames;
	
	state = CAMERA_RECORDING;
	
	ofLog(OF_LOG_VERBOSE, "ofxiVideoGrabber::record, first frame: %i", firstFrame);
	
}

void ofxiVideoGrabber::pause() {
	state = CAMERA_RUNNING;
}

bool ofxiVideoGrabber::cameraToggle() {
	return [videoTexture cameraToggle] == YES;
}


int ofxiVideoGrabber::getCameraWidth() {
	return 480;
}

int ofxiVideoGrabber::getCameraHeight() {
	float w = 480; // normalizing each camera to 480 * 360
	return  (float)videoTexture.videoDimensions.height / (float)videoTexture.videoDimensions.width * w ;

}



bool ofxiVideoGrabber::getIsReady() {
	return videoTexture.CameraTexture;
}

void ofxiVideoGrabber::setOffset(ofPoint &offset) {
	this->offset = offset;
	
	
	float w = 480; // normalizing each camera to 480 * 360
	float h = (float)videoTexture.videoDimensions.height / (float)videoTexture.videoDimensions.width * w ;
	
	rect.x = offset.x/w;
	rect.y = offset.y/h;
	rect.width = (float)video->textureWidth / w;
	rect.height = (float)video->textureHeight / h;
	
}



