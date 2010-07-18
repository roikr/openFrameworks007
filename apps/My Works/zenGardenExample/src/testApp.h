#pragma once

#include "ofMain.h"
#include "ZenGarden.h"

class testApp : public ofBaseApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed( int key );
		
	void audioRequested 	(float * input, int bufferSize, int nChannels);
	
	
	static const float sampleRate = 44100.0f;

	
	
	static const int blockSize = 256;
	static const int numInputChannels = 0;
	static const int numOutputChannels = 2;
	
	
	float *inputBuffers;
	float *outputBuffers;
	
	PdGraph *graph;
	
	bool bKey;
	
	
	
		
};

