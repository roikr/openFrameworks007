/*
 *  ofxAudioLimiter.cpp
 *  audioLimiterExample
 *
 *  Created by Roee Kremer on 8/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxAudioLimiter.h"
#include <cmath>
#include "ofMain.h"

//attack and release in milliseconds
void ofxAudioLimiter::setup(float attackMs,float releaseMs,float sampleRate,float thresh) {
	
//	a = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) );
//	r = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) );
	
	ga = (float) exp(-1/(sampleRate*attackMs/1000));
	gr = (float) exp(-1/(sampleRate*releaseMs/1000));
	
	envelope[0] = 0;
	envelope[1] = 0;
}


void ofxAudioLimiter::audioProcess( float * buffer, int bufferSize, int nChannels ) {
	for (int j=0; j<nChannels; j++) {
		for (int i = 0; i < bufferSize; i++){
			
			
			
			
			//get your data into 'input'
			float EnvIn = fabs(buffer[i*nChannels+j]);
			
			if(envelope[j] < EnvIn)
			{
				envelope[j] *= ga;
				envelope[j] += (1-ga)*EnvIn;
			}
			else
			{
				envelope[j] *= gr;
				envelope[j] += (1-gr)*EnvIn;
			}
			//envelope now contains.........the envelope ;)
			
		}
		
		if (envelope[j]>thresh) {
			
			for (int i = 0; i < bufferSize; i++){
				buffer[i*nChannels+j]/=envelope[j]+(1-thresh);
			}
		}
		
	}
}


void ofxAudioLimiter::draw() {
	
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	ofEnableAlphaBlending();
	
	ofFill();
	ofRect(0, (1-envelope[0])* ofGetHeight(), ofGetWidth()/2, envelope[0]*ofGetHeight());
	ofRect(ofGetWidth()/2, (1-envelope[1])* ofGetHeight(), ofGetWidth(), envelope[0]*ofGetHeight());
	
	
	
}
