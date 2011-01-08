/*
 *  ofxiVideoLoader.h
 *  VideoToTextures
 *
 *  Created by Roee Kremer on 12/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofxiPhoneVideo.h"
#include "ofxiFBO.h"
#include <string>


class ofxiVideoLoader {
	
public:
	
	void setup();
	
	
	
	void addVideo(ofxiPhoneVideo *video);
	void loadVideo(string filename);
	
	void loadAudio(ofxiPhoneVideo *video,string filename);
	
	GLuint createVideoTextue(GLuint w,GLuint h);
	
	vector<ofxiPhoneVideo*> videos;
	
	
};
