/*
 *  ofxAudioLimiter.h
 *  audioLimiterExample
 *
 *  Created by Roee Kremer on 8/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once


class ofxAudioLimiter {
	
public:
	void setup(float attackMs,float releaseMs,float sampleRate,float thresh);
	void audioProcess( float * buffer, int bufferSize, int nChannels );
	void draw();
	
private:
	float ga;
	float gr;
	float envelope[2];
	float thresh;
		
	
};