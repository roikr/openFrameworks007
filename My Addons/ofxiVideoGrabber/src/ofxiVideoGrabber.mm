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

#define LOG_VIDEO_GRABBER

ofxiVideoGrabber::ofxiVideoGrabber():state(CAMERA_NONE),offset(ofPoint(0,0)),bStartCapture(false) {
}

int ofxiVideoGrabber::getState() {
	return state;
}
	

void ofxiVideoGrabber::setup(ofxiPhoneVideo *video,int cameraPosition,float scale) {
	
	videoTexture = [[[MyVideoBuffer alloc] initWithFPS:video->fps devicePosition:cameraPosition == FRONT_CAMERA ? AVCaptureDevicePositionFront : AVCaptureDevicePositionBack] retain];
	
	
	
//	fbo.setup();
	
	fbo.setup(video->textureWidth, video->textureHeight);
	
	this->video = video;
	this->scale = scale;
    
    state = CAMERA_READY;
}

void ofxiVideoGrabber::setOffset(ofPoint &offset) {
    
	this->offset = offset;
	bOffsetChanged = true;
	
}

void ofxiVideoGrabber::update() {
    if (state==CAMERA_STARTED && videoTexture.CameraTexture) {
#ifdef LOG_VIDEO_GRABBER
        printf("CAMERA_RUNNING\n");
#endif
        state=CAMERA_RUNNING;
    }
    
    if (state>=CAMERA_RUNNING && bOffsetChanged) {
#ifdef LOG_VIDEO_GRABBER
        printf("OffsetChanged\n");
#endif
        bOffsetChanged = false;
        float w = 480; // normalizing each camera to 480 * 360
        float h = (float)videoTexture.videoDimensions.height / (float)videoTexture.videoDimensions.width * w ;
        
        rect.x = offset.x/w;
        rect.y = offset.y/h;
        rect.width = (float)video->textureWidth / w;
        rect.height = (float)video->textureHeight / h;

    }
    
    if (state==CAMERA_RUNNING && bStartCapture) {
#ifdef LOG_VIDEO_GRABBER
        printf("CAMERA_CAPTURING\n");
#endif
        bStartCapture = false;
        if (video->textures.empty()) {
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
        
        state = CAMERA_CAPTURING;
        
        currentFrame = 0;
        
        cameraFrame = videoTexture.currentFrame-1;
    }

}


void ofxiVideoGrabber::render() {
    
	
	if (state >= CAMERA_CAPTURING) {
		
		if  (cameraFrame != videoTexture.currentFrame ) {
		
			cameraFrame++;
			
			if (videoTexture.currentFrame - cameraFrame > 0) {
				NSLog(@"skip %i frames",videoTexture.currentFrame - cameraFrame );
			}
			
			GLuint texture = video->textures[currentFrame % video->numFrames];
			fbo.begin(texture);	
			
			glPushMatrix();
			glScalef(scale, scale, 1.0);
			drawCamera();
			glPopMatrix();
			
			
			currentFrame++;
			
			if (state == CAMERA_RECORDING) {
				if ( currentFrame-1 - firstFrame >= video->numFrames - video->numIntroFrames-1)  {
					stopCapture();
				}
			}
			
			
			fbo.end();
		}
	}
}


void ofxiVideoGrabber::draw() {
    if (state==CAMERA_RUNNING) {
        glPushMatrix();
		glScalef(scale, scale, 1.0);
		drawCamera();
		glPopMatrix();
    }
	
    if (state >= CAMERA_CAPTURING) {
		drawTexture(video->textures[(currentFrame-1) % video->numFrames]);
	} 
	
	//	if (video->bMirrored) {
	//		glTranslatef(video->width, 0, 0);
	//		glScalef(-1.0, 1.0, 1.0);
	//	}
	
}


void ofxiVideoGrabber::suspend() {
    
    if (state>=CAMERA_CAPTURING) {
        stopCapture();
    }
	
    if (state>CAMERA_NONE) {
        for (vector<int>::iterator iter=video->textures.begin(); iter!=video->textures.end(); iter++) {
            GLuint texture = *iter;
            glDeleteTextures(1, &texture);
            printf("delete camera texture: %i\n",texture);
        }
        video->textures.clear();
    }
	
	
}

	

void ofxiVideoGrabber::exit() {
    if (state==CAMERA_NONE) {
        return;
    }
	stopCamera();
	[videoTexture release];
	videoTexture = nil;
	
}


void ofxiVideoGrabber::startCamera() {
    if (state==CAMERA_READY) {
#ifdef LOG_VIDEO_GRABBER
        printf("startCamera\n");
#endif
        [videoTexture._session startRunning];
        state = CAMERA_STARTED;
    }
}

void ofxiVideoGrabber::stopCamera() {
    if (state>=CAMERA_RUNNING) {
    	
        if([videoTexture._session isRunning])
            [videoTexture._session stopRunning];
        
        state = CAMERA_READY;
//	fbo.release();
    }

}


void ofxiVideoGrabber::startCapture() {
    // roikr: need to startCamera before
    bStartCapture = true;
		
}

void ofxiVideoGrabber::stopCapture() {
    if (state>=CAMERA_CAPTURING) {
        state = CAMERA_RUNNING;
    }
    // roikr: don't change state when IDLE if so - it won't start (startCamera)
}

void ofxiVideoGrabber::record() {
// roikr: audio fix	
//	video->audio.record();
    
    if (state==CAMERA_CAPTURING) {
        firstFrame = currentFrame-1;
        video->firstFrame = (currentFrame-1) % video->numFrames;
        
        state = CAMERA_RECORDING;
        
        ofLog(OF_LOG_VERBOSE, "ofxiVideoGrabber::record, first frame: %i", firstFrame);

    }
}


bool ofxiVideoGrabber::cameraToggle() {
	return state>=CAMERA_RUNNING ? [videoTexture cameraToggle] == YES : false;
}


int ofxiVideoGrabber::getCameraWidth() {
    
	return state>=CAMERA_RUNNING ? 480 : 0;
}

int ofxiVideoGrabber::getCameraHeight() {
    
	float w = 480; // normalizing each camera to 480 * 360
	return  state>=CAMERA_RUNNING ?  (float)videoTexture.videoDimensions.height / (float)videoTexture.videoDimensions.width * w : 0;

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
	
    
	glEnable(GL_TEXTURE_2D);
	
	// bind the texture
	glBindTexture(GL_TEXTURE_2D, videoTexture.CameraTexture );
	
	GLfloat px0 = 0;		// up to you to get the aspect ratio right
	GLfloat py0 = 0;
	GLfloat px1 = video->textureWidth/scale;
	GLfloat py1 = video->textureHeight/scale;
	
	
	GLfloat tx0 = rect.x;
	GLfloat ty0 = rect.y+rect.height/scale;
	GLfloat tx1 = rect.x+rect.width/scale;
	GLfloat ty1 = rect.y;
	
	if (videoTexture.devicePosition == AVCaptureDevicePositionBack) {
		ty0 = rect.y;
		ty1 = rect.y+rect.height/scale;
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


