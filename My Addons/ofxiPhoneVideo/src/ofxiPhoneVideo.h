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
	int		fps;
	int		sampleRate;
	int		sampleLength; // ms
	int		numFrames;
	int		numBuffers;
	int		bufferSize;
	
	vector<int> textures;
	float	*sample;
	
	int width;
	int height;
};