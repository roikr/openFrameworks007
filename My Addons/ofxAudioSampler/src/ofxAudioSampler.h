/*
 *  ofxAudioSampler.h
 *  iPhoneDynamicCompressor
 *
 *  Created by Roee Kremer on 12/12/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


class ofxAudioSampler {
	
public:
	ofxAudioSampler();
	void setup(int bufferSize,int numBuffers);
	void draw();
	
	void audioReceived( float * input, int bufferSize);
	void audioRequested( float * output, int bufferSize);
	void mix(float *buffer,int bufferSize,float volume=1.0f);
	
	void record();
	void play(float speed);
	void stop();
	
	void normalize();
	
	
	bool getIsPlaying();
	bool getIsRecording();
	
	float *getBuffer();
	int getCurrentBuffer();
	int getBufferSize();
	int getNumBuffers();
	
	int bufferSize;
	int numBuffers;
	int state;
	
	float *buffer;
	int currentBuffer;
	
	int pos;
	float speed;
};