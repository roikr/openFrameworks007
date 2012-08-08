//
//  ofxiVideoPlayer.cpp
//  videoPlayerExample
//
//  Created by Roee Kremer on 5/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiVideoPlayer.h"
#include "VideoPlayer.h"
#include "ofxiPhoneExtras.h"


#define PRE_SAMPLES 4096

void ofxiVideoPlayer::play(string filename) {
    if (bAllocated) {
        return;
    }
    
    if (ofFile(filename).exists()) {
        bAllocated = true;
        videoPlayer = [[VideoPlayer alloc] initWithURL:[NSURL fileURLWithPath:[NSString stringWithCString:filename.c_str()]] context:[ofxiPhoneGetGLView() context]]; 
        
        
        playPos = 0;
        bPlaying = false;
    }
    
	
    
}

void ofxiVideoPlayer::update() {
	
    if (videoPlayer == NULL) {
        return;
    }
	
    
	if (!bPlaying) {
		[videoPlayer updateVideo];
		if (videoPlayer.state == VIDEO_PLAYER_STATE_PLAYING) {
			bPlaying = true;
			currentFrame = 0;
			startTime=ofGetElapsedTimeMillis();
		}
	} else {
		if (!getFrameRate()) {
			[videoPlayer updateVideo];
			currentFrame++;
		} else if ( int((ofGetElapsedTimeMillis()-startTime) *  getFrameRate()/ 1000) > currentFrame) {
			[videoPlayer updateVideo];
			currentFrame++;
		}
        
        if (videoPlayer.state == VIDEO_PLAYER_STATE_FINISHED) {
            bPlaying = false;
            [videoPlayer release];
            videoPlayer = NULL;
            bAllocated = false;
            
        }
        
	}
    
    if (bPlaying) {
        while (videoPlayer.readPos-playPos<PRE_SAMPLES && [videoPlayer readAudio]  );
		
	} 
	
	//printf("readPos: %i, playPos: %i\n",videoPlayer.readPos,playPos);
}


int ofxiVideoPlayer::getCurrentFrame() {
	return currentFrame;
}

int ofxiVideoPlayer::getElapsedFrame() {
	return  (ofGetElapsedTimeMillis()-startTime) *  getFrameRate()/ 1000;
}

void ofxiVideoPlayer::draw(ofRectangle rect,ofRectangle tex) {
	if (videoPlayer!=NULL && videoPlayer.state != VIDEO_PLAYER_STATE_PLAYING) {
		return;
	}
	
	glPushMatrix();
	
	//if (!bHorizontal) {
    //		ofTranslate(textureHeight, 0, 0);
    //		ofRotateZ(90);
    //		
    //	}
	
	
	
	GLfloat spriteTexcoords[] = {
		tex.x+tex.width,tex.y+tex.height,   
		tex.x+tex.width,tex.y,
		tex.x,tex.y+tex.height,   
		tex.x,tex.y,};
	
    //	float w =videoPlayer.width*u;//ofGetWidth()/2;
    //	float h =videoPlayer.height*v;//(float)ofGetWidth()/(float)width*(float)height/2;
	
	GLfloat spriteVertices[] =  {
		rect.x+rect.width,rect.y+rect.height,0,   
		rect.x+rect.width,rect.y,0,   
		rect.x,rect.y+rect.height,0, 
		rect.x,rect.y,0};
	
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
    
    glEnable(GL_TEXTURE_2D);
    [videoPlayer bind];
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	[videoPlayer unbind];
	
	
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
}

//int ofxiVideoPlayer::getTexture() {
//	
//	return videoPlayer.videoTexture;
//}

void ofxiVideoPlayer::exit() {
	
}


float ofxiVideoPlayer::getFrameRate() {
	return bPlaying ? videoPlayer.frameRate : 0;
}


float ofxiVideoPlayer::getWidth() {
	return bPlaying ? videoPlayer.width : 0;
}

float ofxiVideoPlayer::getHeight() {
	return bPlaying ? videoPlayer.height : 0;
}

bool ofxiVideoPlayer::getIsPlaying() {
	return videoPlayer!=NULL;
}

bool ofxiVideoPlayer::getIsFrameVisible() {
    return videoPlayer!=NULL && videoPlayer.state == VIDEO_PLAYER_STATE_PLAYING;
}

void ofxiVideoPlayer::stop() {
    if (bPlaying) {
        [videoPlayer stop];
    }
}


void ofxiVideoPlayer::audioRequested( float * output, int bufferSize, int nChannels ) {
	
    memset(output, 0, nChannels*bufferSize*sizeof(float));
    
    if (bPlaying) {
//        if (videoPlayer!=NULL  && videoPlayer.bReadAudio && videoPlayer.state == VIDEO_PLAYER_STATE_PLAYING )
        if (videoPlayer.bReadAudio && videoPlayer.readPos-playPos>=bufferSize && videoPlayer.frame>0) {
            for (int i = 0; i < bufferSize; i++){
                
                output[i*nChannels] = videoPlayer.buffer[(playPos+i*videoPlayer.numChannels) % RING_BUFFER_SIZE];// * gain;
                output[i*nChannels + 1] = videoPlayer.numChannels == 2 ? videoPlayer.buffer[(playPos+i*videoPlayer.numChannels+1) % RING_BUFFER_SIZE] : output[i*nChannels];// * gain;
                
            }
            
            playPos+=bufferSize*videoPlayer.numChannels;
        }
    }
	
}
