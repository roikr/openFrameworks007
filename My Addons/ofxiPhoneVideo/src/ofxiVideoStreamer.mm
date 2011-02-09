/*
 *  ofxiVideoStreamer.cpp
 *  kobiPhone
 *
 *  Created by Roee Kremer on 1/23/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiVideoStreamer.h"
#include "VideoReader.h"


#define PRE_SAMPLES 4096

void ofxiVideoStreamer::setup(string filename) {
	videoReader = [[[VideoReader alloc] initWithURL:[NSURL fileURLWithPath:[NSString stringWithCString:filename.c_str()]]] retain]; 
	
	//NSString *str = @"http://www.roikr.com/can.m4v";
	//videoReader = [[[VideoReader alloc] initWithURL:[NSURL URLWithString:str]] retain];
	playPos = 0;

}

void ofxiVideoStreamer::update() {
	
	
	if (!bStreaming) {
		[videoReader updateVideo];
		if (videoReader.videoTexture) {
			bStreaming = true;
			currentFrame = 0;
			startTime=ofGetElapsedTimeMillis();
		}
	} else {
		if (!getFrameRate()) {
			[videoReader updateVideo];
			currentFrame++;
		} else if ( int((ofGetElapsedTimeMillis()-startTime) *  getFrameRate()/ 1000) > currentFrame) {
			[videoReader updateVideo];
			currentFrame++;
			
			
		}
	}
	while (videoReader.readPos-playPos<PRE_SAMPLES && [videoReader readAudio]  ) {
		
	} 
	
	//printf("readPos: %i, playPos: %i\n",videoReader.readPos,playPos);
}


int ofxiVideoStreamer::getCurrentFrame() {
	return currentFrame;
}

int ofxiVideoStreamer::getElapsedFrame() {
	return  (ofGetElapsedTimeMillis()-startTime) *  getFrameRate()/ 1000;
}

void ofxiVideoStreamer::draw(ofRectangle rect,ofRectangle tex) {
	if (!getIsStreaming()) {
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
	
//	float w =videoReader.width*u;//ofGetWidth()/2;
//	float h =videoReader.height*v;//(float)ofGetWidth()/(float)width*(float)height/2;
	
	GLfloat spriteVertices[] =  {
		rect.x+rect.width,rect.y+rect.height,0,   
		rect.x+rect.width,rect.y,0,   
		rect.x,rect.y+rect.height,0, 
		rect.x,rect.y,0};
	
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, spriteVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, spriteTexcoords);	
	glBindTexture(GL_TEXTURE_2D, videoReader.videoTexture);
	glEnable(GL_TEXTURE_2D);
	
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
}

int ofxiVideoStreamer::getTexture() {
	
	return videoReader.videoTexture;
}

void ofxiVideoStreamer::exit() {
	
}


float ofxiVideoStreamer::getFrameRate() {
	return videoReader.frameRate;
}


float ofxiVideoStreamer::getWidth() {
	return videoReader.width;
}

float ofxiVideoStreamer::getHeight() {
	return videoReader.height;
}

bool ofxiVideoStreamer::getIsStreaming() {
	return bStreaming;
}



void ofxiVideoStreamer::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	
	
	if (videoReader.readPos-playPos>=bufferSize && videoReader.frame>0) {
		for (int i = 0; i < bufferSize; i++){
			
			output[i*nChannels] = videoReader.buffer[(playPos+i*videoReader.numChannels) % RING_BUFFER_SIZE];// * gain;
			output[i*nChannels + 1] = videoReader.numChannels == 2 ? videoReader.buffer[(playPos+i*videoReader.numChannels+1) % RING_BUFFER_SIZE] : output[i*nChannels];// * gain;
			
		}
		
		playPos+=bufferSize*videoReader.numChannels;
	} else {
		memset(output, 0, nChannels*bufferSize*sizeof(float));

	}
}

