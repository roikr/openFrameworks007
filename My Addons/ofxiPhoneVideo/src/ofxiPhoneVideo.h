/*
 *  ofxiPhoneVideo.h
 *  iPhoneVideoExample
 *
 *  Created by Roee Kremer on 12/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include <vector>


// for now supporting 44100 mono audio

using namespace std;

struct ofxiPhoneVideo {
	
//public:
//	ofxiPhoneVideo() : bHorizontal(true),bFlipHoriznotal(false) {};
	
	int		numFrames;
	int		numIntroFrames;
	
	int		fps;
//	int		bufferSize;
//	float	sampleRate;
	int		sampleLength; // in millis -  make sure that there are more frames than needed
	
	
	vector<int> textures;
	
	int firstFrame;
	
	int textureWidth; // power of 2
	int textureHeight;
	
	float widthFraction;
	float heightFraction;

	bool bHorizontal;
	bool bFlipHoriznotal;
	
};