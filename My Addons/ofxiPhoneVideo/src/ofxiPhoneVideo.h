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
#include "ofxAudioSampler.h"

// for now supporting 44100 mono audio

using namespace std;

class ofxiPhoneVideo {
	
public:
	
	void drawFrame(int frame);
	void drawTexture(int texture);
	
	int		numFrames;
	int		numIntroFrames;
	
	int		fps;
	int		bufferSize;
	int		sampleRate;
	int		sampleLength; // in millis -  make sure that there are more frames than needed
	
	
	vector<int> textures;
	
	ofxAudioSampler audio;
	
	int firstFrame;
	
	int textureWidth; // power of 2
	int textureHeight;
	
	float widthFraction;
	float heightFraction;

	
	
};