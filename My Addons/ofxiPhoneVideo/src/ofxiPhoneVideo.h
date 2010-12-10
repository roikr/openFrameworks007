/*
 *  ofxiPhoneVideo.h
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>


using namespace std;

struct ofxiPhoneVideo {
	
	int		numFrames;
	int		numIntroFrames;
	
	int		fps;
	int		sampleRate;
	int		sampleLength; // in millis -  make sure that there are more frames than needed
	
	int		numBuffers;
	int		bufferSize;
	
	vector<int> textures;
	int firstFrame;
	float	*sample;
	
	int width;
	int height;
};