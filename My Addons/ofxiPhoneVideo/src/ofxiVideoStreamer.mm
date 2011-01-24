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


#define PRE_SAMPLES 8192

void ofxiVideoStreamer::setup(string filename) {
	videoReader = [[[VideoReader alloc] initWithURL:[NSURL fileURLWithPath:[NSString stringWithCString:filename.c_str()]]] retain]; 
	playPos = 0;

}

void ofxiVideoStreamer::update() {
	[videoReader updateVideo];
	
	while (videoReader.readPos-playPos<PRE_SAMPLES && [videoReader readAudio]  ) {
		
	} 
	
	printf("readPos: %i, playPos: %i\n",videoReader.readPos,playPos);
}

float ofxiVideoStreamer::getFrameRate() {
	return videoReader.frameRate;
}

void ofxiVideoStreamer::draw() {
	
	
	glPushMatrix();
	
	//if (!bHorizontal) {
//		ofTranslate(textureHeight, 0, 0);
//		ofRotateZ(90);
//		
//	}
	
	float widthFraction = 1.0f;
	float heightFraction = 1.0f;
	
	
	float u = widthFraction;
	float v = heightFraction;
	
	GLfloat spriteTexcoords[] = {
		u,v,   
		u,0.0f,
		0,v,   
		0.0f,0,};
	
	float w =videoReader.width*u;//ofGetWidth()/2;
	float h =videoReader.height*v;//(float)ofGetWidth()/(float)width*(float)height/2;
	
	GLfloat spriteVertices[] =  {
		w,h,0,   
		w,0,0,   
		0,h,0, 
		0,0,0};
	
	
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

void ofxiVideoStreamer::exit() {

}


void ofxiVideoStreamer::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	
	
	if (videoReader.readPos-playPos>=bufferSize && videoReader.frame>0) {
		for (int i = 0; i < bufferSize; i++){
			output[i*nChannels] = videoReader.buffer[(playPos+i) % RING_BUFFER_SIZE];// * gain;
			output[i*nChannels + 1] = videoReader.buffer[(playPos+i) % RING_BUFFER_SIZE];// * gain;
		}
		
		playPos+=bufferSize;
	} else {
		memset(output, 0, nChannels*bufferSize*sizeof(float));

	}
}

