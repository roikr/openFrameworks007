/*
 *  ofxAudioFile.h
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

// ROIKR: need to check stereo support and reading multiple buffers

#ifndef _AUDIO_FILE
#define _AUDIO_FILE

#include <string>
#include <AudioToolbox/AudioToolbox.h>

using namespace std;

//@class AVAssetWriterInput;
//@class AVAssetWriter;

class ofxAudioFile {
public:
	
	ofxAudioFile();
	
	bool load(string filename,int blockLength);
	void setupForSave(int blockLength); // allocate memory just for current block
	void exit();
	float *getTableBuffer();
	int getSamplesPerChannel();
	
	
	void play();
	bool getIsPlaying();
	bool getIsLastBlock();
	bool getIsLastBlock(int block);
	float* getCurrentBlock(int channel);
	float* getBlock(int block,int channel);
	
	void mixWithBlocks(float *left,float *right,float volume=1.0f);
	void mix(float *left,float *right,int block,float volume,bool ramp);
	void postProcess(); // to call after processing
	
	// ROIKR: bug when using audioRequested with 2 channels...
	void audioRequested (float * output, int channel,int bufferSize, int nChannels); // to use without postProcess and for left side only

	void openForSave(string filename);
	void saveWithBlocks(float *left,float*right);
	void close();
	
private:
	bool bLoaded;
	int samplesPerChannel;
	int bufferLength;
	
	int blockLength;
	SInt16 *saveBuffer;
	AudioBufferList fillBufList;
	
	float *tableBuffer;
	int channels;
	bool bIsPlaying;
	int currentBlock;

	ExtAudioFileRef file;
//	AVAssetWriterInput *audioInput;
//	AVAssetWriter *writer;
	
	string filename;
};

#endif 