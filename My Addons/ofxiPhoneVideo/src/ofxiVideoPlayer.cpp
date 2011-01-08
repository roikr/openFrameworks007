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
	PLAYER_PLAYING,
	PLAYER_INTRO_FORWARD,
	PLAYER_INTRO_BACKWARD
};

void ofxiVideoPlayer::setup(ofxiPhoneVideo *video,bool bIntroMode) {

	this->video = video;
	
	state = PLAYER_IDLE;
	this->bIntroMode = bIntroMode;
	
	if (!bIntroMode && video->textures.size()) {
		currentTexture = video->textures[video->firstFrame];

	}
	
}

void ofxiVideoPlayer::seekFrame(int nextFrame) {
	
	
	
	
	switch (state) {
		case PLAYER_PLAYING: {
			if (nextFrame >= video->textures.size()-video->numIntroFrames) {
				state = PLAYER_IDLE;
				if (bIntroMode) {
					playIntro();
				} else {
					currentTexture = video->textures[video->firstFrame];
				}

			} else {
				int temp = video->textures[(video->firstFrame+nextFrame) % video->textures.size()];
				if	(temp!=currentTexture) {
					//printf("seekFrame: %i, texture: %i\n", nextFrame,temp);
					currentTexture =temp;
				}
			}
		}	break;
			
		case PLAYER_INTRO_FORWARD: 
		case PLAYER_INTRO_BACKWARD: {
			
			state = (nextFrame / (video->numIntroFrames - 1)) % 2 ? PLAYER_INTRO_BACKWARD : PLAYER_INTRO_FORWARD;
			int mod = nextFrame % (video->numIntroFrames - 1);
			int frame = (state == PLAYER_INTRO_FORWARD) ?  mod  : (video->numIntroFrames - 1) - mod;
			
			currentTexture = video->textures[(video->textures.size()+video->firstFrame - video->numIntroFrames + frame) % video->textures.size()];
			
			
			
			
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
	
	seekFrame(currentFrame);
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
	video->drawTexture(currentTexture);
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
			
			if (bIntroMode) {
				playIntro();
			} 

		}
	}
	
}

void ofxiVideoPlayer::play(int note,int velocity) {
	
	float ratio = exp((float)(note-60)/12.0*log(2.0));
	float volume = (float)velocity / 127;
	//printf("note:  %i %i, ratio: %1.2f %1.2f\n", note,velocity,ratio,volume);
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




void ofxiVideoPlayer::playIntro() {
	state = PLAYER_INTRO_FORWARD;
	start = ofGetElapsedTimeMillis();
	currentTexture = video->textures[(video->firstFrame-video->numIntroFrames+video->textures.size()) % video->textures.size()];
	currentFrame = 0;
	speed = 1;
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



