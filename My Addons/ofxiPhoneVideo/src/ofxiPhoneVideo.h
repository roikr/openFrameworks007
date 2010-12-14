/*
 *  ofxiPhoneVideo.h
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>
#include "ofxAudioSampler.h"

using namespace std;

struct ofxiPhoneVideo {
	
	int		numFrames;
	int		numIntroFrames;
	
	int		fps;
	int		sampleRate;
	int		sampleLength; // in millis -  make sure that there are more frames than needed
	
	
	vector<int> textures;
	
	ofxAudioSampler audio;
	
	int firstFrame;
	//float	*sample;
	
	int width;
	int height;
	
	bool bMirrored;
};