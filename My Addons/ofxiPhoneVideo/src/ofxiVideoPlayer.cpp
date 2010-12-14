/*
 *  ofxiVideoPlayer.cpp
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiVideoPlayer.h"

#include "ofxiPhoneVideo.h"
#include "ofMain.h"
#include <math.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


//- (uint) numberOfTextures {
//	return textures.size();
//}
//
//- (uint) texture:(uint)frame {
//	return textures.size() ? textures[frame] : 0;
//}

enum {
	PLAYER_IDLE,
	PLAYER_RECORDING,
	PLAYER_PLAYING,
	PLAYER_SCRUBBING
};

void ofxiVideoPlayer::setup(ofxiPhoneVideo *video) {

	this->video = video;
	
	state = PLAYER_IDLE;
	
}

void ofxiVideoPlayer::update() {
		
	if (state == PLAYER_PLAYING) {
		int tempFrame = speed*(ofGetElapsedTimeMillis()-start)*video->fps/1000;
		if (tempFrame >= video->textures.size()-video->numIntroFrames) {
			state = PLAYER_IDLE;
		} else {
			currentTexture =video->textures[(video->firstFrame+tempFrame) % video->textures.size()];
		}
	}
	
}



void ofxiVideoPlayer::draw() {
	drawTexture(currentTexture);
}

void ofxiVideoPlayer::drawFrame(int frame) {
	drawTexture(video->textures[frame]);
}

void ofxiVideoPlayer::drawTexture(int texture) {
		
	
	
	
	GLfloat spriteTexcoords[] = {
		1.0f,1.0f,   
		1.0f,0.0f,
		0,1.0f,   
		0.0f,0,};
	
	float w = video->width;
	float h = video->height;
	
	GLfloat spriteVertices[] =  {
		w,h,0,   
		w,0,0,   
		0,h,0, 
		0,0,0};
	
	
	glPushMatrix();
	if (video->bMirrored) {
		glTranslatef(w, 0, 0);
		glScalef(-1.0, 1.0, 1.0);
	}
	
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
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
		
}

void ofxiVideoPlayer::audioRequested( float * output, int bufferSize) {
	if( video->audio.getBufferSize() != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoPlayer: your buffer size was set to %i - but the stream needs a buffer size of %i", video->audio.getBufferSize(), bufferSize);
		return;
	}
	
	
	if (state == PLAYER_PLAYING) {
		
		
		for (int i = 0; i < bufferSize; i++){
			
			output[i ] = (*(video->audio.getBuffer()+pos+(int)(speed*i))) * this->volume;
		}
		
	} else {
		for (int i = 0; i < bufferSize; i++){
			output[i ] = 0;
		}
	}
	
}

void ofxiVideoPlayer::mix(float *buffer,int bufferSize,float volume) {
	if( video->audio.getBufferSize() != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoPlayer: your buffer size was set to %i - but the stream needs a buffer size of %i", video->audio.getBufferSize(), bufferSize);
		return;
	}
	
	if (state == PLAYER_PLAYING) {
		
		for (int i = 0; i < bufferSize; i++) {
			buffer[i ] += (*(video->audio.getBuffer()+pos+(int)(speed*i))) *volume * this->volume;
		}
		
	} 
}

void ofxiVideoPlayer::preProcess() {
	
	if (state == PLAYER_PLAYING) {
		pos+=video->audio.bufferSize*speed;
		
		if (pos + video->audio.bufferSize*speed >=video->audio.getNumBuffers()*video->audio.bufferSize) {
			state = PLAYER_IDLE;
		}
	}
	
}

void ofxiVideoPlayer::play(int note,int velocity) {
	
	float ratio = exp((float)(note-60)/12.0*log(2.0));
	float volume = (float)velocity / 127;
	printf("note:  %i %i, ratio: %1.2f %1.2f\n", note,velocity,ratio,volume);
	play(ratio,volume);
}

void ofxiVideoPlayer::play(float speed,float volume) {
	state = PLAYER_PLAYING;
	start = ofGetElapsedTimeMillis();
	currentTexture = video->textures[video->firstFrame];
	currentFrame = 0;
	pos = 0;
	this->speed = speed;
	this->volume = volume;
}

bool ofxiVideoPlayer::getIsPlaying() {
	return state == PLAYER_PLAYING;
}


void ofxiVideoPlayer::nextFrame() {
	if (getIsPlaying()) {
		currentFrame++;
		if (currentFrame >= video->textures.size()-1-video->numIntroFrames) {
			state = PLAYER_IDLE;
			currentTexture = video->textures[video->firstFrame];
		} else {
			currentTexture =video->textures[(video->firstFrame+currentFrame) % video->textures.size()];
		}
		
	}
}


void ofxiVideoPlayer::introFrame() {
	
	currentTexture =video->textures[(video->textures.size()+video->firstFrame-video->numIntroFrames) % video->textures.size()];
}

/*
void ofxiVideoPlayer::startScrubbing() {
	state = PLAYER_SCRUBBING;
}

void ofxiVideoPlayer::stopScrubbing() {
	state = PLAYER_IDLE;
}

bool ofxiVideoPlayer::getIsScrubbing() {
	return state == PLAYER_SCRUBBING;
}


void ofxiVideoPlayer::setPosition(float pos) {
	currentTexture = pos*(int)video->textures.size();
}
 */



