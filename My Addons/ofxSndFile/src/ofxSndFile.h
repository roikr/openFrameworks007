/*
 *  ofxSndFile.h
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SND_FILE
#define _SND_FILE

#include <string>

using namespace std;

class ofxSndFile {
public:
	
	bool load(string filename,int blockLength);
	void setupForSave(int blockLength); // allocate memory just for current block
	void exit();
	float *getTableBuffer();
	int getSamplesPerChannel();
	
	
	void play();
	bool getIsPlaying();
	bool getIsLastBlock();
	float* getCurrentBlock(int channel);
	
	void mixWithBlocks(float *left,float *right,float volume=1.0f);
	void postProcess(); // to call after processing

	void open(string filename);
	void saveWithBlocks(float *left,float*right);
	void close();
	
private:
	
	int samplesPerChannel;
	int bufferLength;
	
	int blockLength;
	
	float *tableBuffer;
	int channels;
	bool bLoaded;
	bool bIsPlaying;
	int currentBlock;
	void *sndFilePtr;
	
	
};

#endif 