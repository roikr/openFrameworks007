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
#include <deque>
#include <AudioToolbox/AudioToolbox.h>

using namespace std;

//@class AVAssetWriterInput;
//@class AVAssetWriter;

struct instance {
	int pos;
	float volume;
	float speed;
	bool bStop;
};


class ofxAudioFile {
public:
	
	ofxAudioFile();
	
	bool load(string filename,int bufferLength);
	void setupForSave(int bufferLength); // allocate memory just for current block
	void exit();
    int getNumChannels();
	int getSamplesPerChannel();
	
	
	void trigger(float speed,float volume,bool retrigger);
	void play();
	void stop();
	
	
	//void mixWithBlocks(float *left,float *right,float volume=1.0f);
	//void channelRequested(float * output, int channel, int nChannels,float volume=1.0f);

	// ROIKR: bug when using audioRequested with 2 channels...
	//void audioRequested (float * output, int channel,int bufferSize, int nChannels); 
		
	//void mix(float *left,float *right,int block,float volume,bool ramp);
	void mixChannel(float * output, int channel, int nChannels);
	void postProcess(); // to call after processing

	

	void openForSave(string filename);
	void save(float * output,int nChannels);
	void close();
	
	int getNumPlaying();
    
	float *getTableBuffer();
private:
	
	
	//float* getCurrentBlock(int channel);
	//float* getBlock(int block,int channel);
	
	bool bLoaded;
	int samplesPerChannel;
	int bufferLength;
	
	SInt16 *saveBuffer;
	AudioBufferList fillBufList;
	
	float *tableBuffer;
	int channels;
	
	ExtAudioFileRef file;
//	AVAssetWriterInput *audioInput;
//	AVAssetWriter *writer;
	
	string filename;
	deque<instance > instances;
};

#endif 