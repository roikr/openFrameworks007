/*
 *  ofxiVideoPlayer.cpp
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiVideoPlayer.h"
#include "ofMain.h"
#include <math.h>
#include "ofxiPhoneVideo.h"

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
	PLAYER_PLAYING,
	PLAYER_INTRO_FORWARD,
	PLAYER_INTRO_BACKWARD
};

void ofxiVideoPlayer::setup(ofxiPhoneVideo *video,bool bIntroMode,float introSpeed) {

	this->video = video;
//	this->buffer = buffer;
//	this->bufferSize = bufferSize;
//	this->numBuffers = numBuffers;
	
	state = PLAYER_IDLE;
	this->bIntroMode = bIntroMode;
	this->introSpeed = introSpeed;
	
	if (!bIntroMode && video->textures.size()) {
		currentTexture = video->textures[video->firstFrame];

	}
	
}

void ofxiVideoPlayer::seekFrame(int frame) {
	
	
	
	
	switch (state) {
		case PLAYER_PLAYING: {
			if (frame >= video->textures.size()-video->numIntroFrames) {
				state = PLAYER_IDLE;
				if (bIntroMode) {
					playIntro();
				} else {
					currentTexture = video->textures[video->firstFrame];
				}

			} else {
				int temp = video->textures[(video->firstFrame+frame) % video->textures.size()];
				if	(temp!=currentTexture) {
					//printf("seekFrame: %i, texture: %i\n", nextFrame,temp);
					currentTexture =temp;
				}
			}
		}	break;
			
		case PLAYER_INTRO_FORWARD: 
		case PLAYER_INTRO_BACKWARD: {
			
			state = (frame / (video->numIntroFrames - 1)) % 2 ? PLAYER_INTRO_BACKWARD : PLAYER_INTRO_FORWARD;
			int mod = frame % (video->numIntroFrames - 1);
			int introFrame = (state == PLAYER_INTRO_FORWARD) ?  mod  : (video->numIntroFrames - 1) - mod;
			
			currentTexture = video->textures[(video->textures.size()+video->firstFrame - video->numIntroFrames + introFrame) % video->textures.size()];
			
			
			
			
		} break;
			
		default:
			break;
	}
}

void ofxiVideoPlayer::update() {
	
	if (state == PLAYER_IDLE) {
		return;
	}
	
	seekFrame(speed*(ofGetElapsedTimeMillis()-start)*video->fps/1000);
				
}

void ofxiVideoPlayer::updateFrame() {
	if (state == PLAYER_IDLE) {
		
		return;
	}
	
	seekFrame(speed*currentFrame);
	currentFrame++;
	
	
	//	if (getIsPlaying()) {
	//		currentFrame++;
	//		if (currentFrame >= video->textures.size()-1-video->numIntroFrames) {
	//			state = PLAYER_IDLE;
	//			currentTexture = video->textures[video->firstFrame];
	//		} else {
	//			currentTexture =video->textures[(video->firstFrame+currentFrame) % video->textures.size()];
	//		}
	//	}
	
	
}


void ofxiVideoPlayer::draw() {
	drawTexture(currentTexture);
}



void ofxiVideoPlayer::drawFrame(int frame) {
    if (video->textures.size())
        drawTexture(video->textures[frame]);
}

void ofxiVideoPlayer::drawTexture(int texture) {
	
	
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
	
	float w = video->textureWidth*u;
	float h = video->textureHeight*v;
	
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
	
	
	glDisable(GL_TEXTURE_2D);
	
	//	glPushMatrix();
	//	//	if (video->bMirrored) {
	//	//		glTranslatef(w, 0, 0);
	//	//		glScalef(-1.0, 1.0, 1.0);
	//	//	}
	//	
	//	glPopMatrix();
	
}


/*

void ofxiVideoPlayer::audioRequested( float * output, int bufferSize) {
	if( this->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoPlayer: your buffer size was set to %i - but the stream needs a buffer size of %i", this->bufferSize, bufferSize);
		return;
	}
	
	
	if (state == PLAYER_PLAYING) {
		
		
		for (int i = 0; i < bufferSize; i++){
			
			output[i ] = (*(buffer+pos+(int)(speed*i))) * this->volume;
		}
		
	} else {
		for (int i = 0; i < bufferSize; i++){
			output[i ] = 0;
		}
	}
	
}

// void ofxiVideoPlayer::mix(float *buffer,int bufferSize,float volume) 
void ofxiVideoPlayer::mixChannel(float * output, int bufferSize,int channel, int nChannels,float volume) {

	if( this->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "ofxiVideoPlayer: your buffer size was set to %i - but the stream needs a buffer size of %i", this->bufferSize, bufferSize);
		return;
	}
	
	if (state == PLAYER_PLAYING) {
		for (int i = 0; i < bufferSize; i++) {
			output[i*nChannels+channel] += (*(buffer+pos+(int)(speed*i))) *volume * this->volume;
		}
		
	} 
}

void ofxiVideoPlayer::preProcess() {
	
	if (state == PLAYER_PLAYING) {
		pos+=bufferSize*speed;
		
		if (pos + bufferSize*speed >=numBuffers*bufferSize) {
			state = PLAYER_IDLE;	
			
			if (bIntroMode) {
				playIntro();
			} 

		}
	}
	
}
 
*/

//void ofxiVideoPlayer::play(int note,int velocity) {
//	
//	float ratio = exp((float)(note-60)/12.0*log(2.0));
//	float volume = (float)velocity / 127;
//	//printf("note:  %i %i, ratio: %1.2f %1.2f\n", note,velocity,ratio,volume);
//	play(ratio,volume);
//}

void ofxiVideoPlayer::play(float speed) {
    if (video->textures.size()) {
    
        state = PLAYER_PLAYING;
        start = ofGetElapsedTimeMillis();
        currentTexture = video->textures[video->firstFrame];
        currentFrame = 0;
    //	pos = 0;
        this->speed = speed;
    //	this->volume = volume;
    }
}

bool ofxiVideoPlayer::getIsPlaying() {
	return state == PLAYER_PLAYING;
}




void ofxiVideoPlayer::playIntro() {
    if (video->textures.size()) {
    
        state = PLAYER_INTRO_FORWARD;
        start = ofGetElapsedTimeMillis();
        
        int introFirst = (video->firstFrame-video->numIntroFrames+video->textures.size()) % video->textures.size();
        currentTexture = video->textures[introFirst];
    //	ofLog(OF_LOG_VERBOSE, "playIntro: first: %i, introFrames: %i, size: %i, introFirst: %i, currentTexture: %i",video->firstFrame,video->numIntroFrames,video->textures.size(),introFirst,currentTexture);
        currentFrame = 0;
        speed = introSpeed;
    }
}


void ofxiVideoPlayer::introFrame() {
	if (video->textures.size()) {
        currentTexture =video->textures[(video->textures.size()+video->firstFrame-video->numIntroFrames) % video->textures.size()];

    }
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



