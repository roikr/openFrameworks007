/*
 *  ofxiPhoneVideo.cpp
 *  SingingCard
 *
 *  Created by Roee Kremer on 12/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "ofxiPhoneVideo.h"
#include "ofMain.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

void ofxiPhoneVideo::drawFrame(int frame) {
	drawTexture(textures[frame]);
}

void ofxiPhoneVideo::drawTexture(int texture) {
	
	glPushMatrix();
	
	if (!bHorizontal) {
		ofTranslate(textureHeight, 0, 0);
		ofRotateZ(90);
		
	}
	
	float u = widthFraction;
	float v = heightFraction;
	
	GLfloat spriteTexcoords[] = {
		u,v,   
		u,0.0f,
		0,v,   
		0.0f,0,};
	
	float w = textureWidth;//ofGetWidth()/2;
	float h = textureHeight;//(float)ofGetWidth()/(float)width*(float)height/2;
	
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
	glPopMatrix();
	
}
