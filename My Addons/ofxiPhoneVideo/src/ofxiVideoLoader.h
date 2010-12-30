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
#include <string>

class ofxiVideoLoader {
	
public:
	
	void setup(ofxiPhoneVideo *video);
	void load(string filename);
	
	ofxiPhoneVideo *video;
	
};
