/*
 *  ofxAudioCompressor.h
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

class ofxAudioCompressor {
	
public:
	ofxAudioCompressor();
	void setup();
	void draw();
	void audioProcess( float * input, float *output,int bufferSize);
	void audioProcess( float * buffer, int bufferSize);

};