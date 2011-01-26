/*
 *  ofxSndFile.cpp
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSndFile.h"

#include "sndfile.h"
#include <iostream>

bool ofxSndFile::load(string filename,int blockLength) {
	
	bIsPlaying = false;
	this->blockLength = blockLength;
		
	//char *newStr = (char *) malloc((filename.size()+1) * sizeof(char));
	//strcpy(newStr, filename.c_str());

	
	SF_INFO sfInfo;
	
	SNDFILE *sndFile = sf_open(filename.c_str(), SFM_READ, &sfInfo);
	
	if (sndFile == NULL) {
		bLoaded = false;
		return false;
	} else {
		bLoaded = true;
	}
		   
	//free(newStr);

	// It is assumed that the channels are interleaved.
	samplesPerChannel = sfInfo.frames;
	bufferLength = samplesPerChannel * sfInfo.channels;
	// create a buffer in memory for the file data
	float *buffer = (float *) malloc(bufferLength * sizeof(float));
	
	
	//sf_count_t res = 
	sf_read_float(sndFile, buffer, bufferLength); // read the whole file into memory
	sf_close(sndFile); // release the handle to the file
	
	//for (int k=0; k<samplesPerChannel; k++) {
//		printf("%0.8f\t", buffer[k]);
//	}
	
	channels = sfInfo.channels;
	
	switch (channels) {
		case 1:
			tableBuffer = buffer;
			buffer = 0; // TODO: not a leak
			break;
		case 2:
			tableBuffer = (float *) malloc(bufferLength * sizeof(float));
			for (int i = 0, j = 0; i < bufferLength; i+=sfInfo.channels, j++) {
				tableBuffer[j] = buffer[i];
				tableBuffer[j+samplesPerChannel] = buffer[i+1];
			}
			free(buffer);
			break;
		default:
			free(buffer);
			break;
	}
	
	return true;
}	

void ofxSndFile::setupForSave(int blockLength) {
	this->blockLength = blockLength;
	this->samplesPerChannel = 0;
	bIsPlaying = false;
	bufferLength = blockLength * 2;
	tableBuffer = (float *) malloc(bufferLength * sizeof(float));
}

float *ofxSndFile::getTableBuffer() {
	return tableBuffer;
}

int ofxSndFile::getSamplesPerChannel() {
	return samplesPerChannel;
}


void ofxSndFile::play() {
	currentBlock = 0;
	bIsPlaying = true;
}

bool ofxSndFile::getIsPlaying() {
	return bIsPlaying;
}

bool ofxSndFile::getIsLastBlock() {
	return (currentBlock+1) * blockLength >= samplesPerChannel;
}

bool ofxSndFile::getIsLastBlock(int block) {
	
	return (block+1) * blockLength >= samplesPerChannel;;
}

float* ofxSndFile::getCurrentBlock(int channel) {
	return (tableBuffer+samplesPerChannel*channel+currentBlock*blockLength);
}

float* ofxSndFile::getBlock(int block,int channel) {
	return (tableBuffer+samplesPerChannel*channel+block*blockLength);
}



void ofxSndFile::mixWithBlocks(float *left,float *right,float volume) {
	if (!bIsPlaying) {
		return;
	} else {
		
		int n = getIsLastBlock() ? (currentBlock+1) * blockLength - samplesPerChannel : blockLength;
		
		float *channel0 = getCurrentBlock(0);
		float *channel1 = channels == 2 ? getCurrentBlock(1) : channel0;
		
			
		for (int i=0; i<n; i++) {
			left[i]+=channel0[i]*volume;
			right[i]+=channel1[i]*volume;
		}
		
	}
}

void ofxSndFile::mix(float *left,float *right,int block,float volume,bool ramp) {
	int n = getIsLastBlock(block) ? (block+1) * blockLength - samplesPerChannel : blockLength;
	
	float *channel0 = getBlock(block,0);
	float *channel1 = channels == 2 ? getBlock(block,1) : channel0;
	
	if (ramp) {
		float step = 1.0/(n-1);
		cout << "ramp" << endl; //<< step << endl;
		for (int i=0; i<n; i++) {
			left[i]+=channel0[i]*volume*((n-1-i)*step);
			right[i]+=channel1[i]*volume*((n-1-i)*step);
		}
	} else {
		for (int i=0; i<n; i++) {
			left[i]+=channel0[i]*volume;
			right[i]+=channel1[i]*volume;
		}
	}
}

void ofxSndFile::postProcess() {
	if (!bIsPlaying)
		return;
	
	
	if (getIsLastBlock()) {
		bIsPlaying = false;
	}
	else
		currentBlock++;
	
}


void ofxSndFile::audioRequested (float * output, int channel,int bufferSize, int nChannels) {
	if (!bIsPlaying ) {
		return;
	} else {
		
		int n = getIsLastBlock() ? (currentBlock+1) * blockLength - samplesPerChannel : blockLength;
		
		
		float *buffer = getCurrentBlock(0);
		for (int i=0; i<n; i++) {
			output[i*nChannels+channel]=buffer[i];
		}
		
		for (int i=n; i<bufferSize; i++) {
			output[i*nChannels+channel]=0;
		}
		
		
		if (getIsLastBlock()) {
			bIsPlaying = false;
		}
		else
			currentBlock++;
		
	}
	
}

void ofxSndFile::open(string filename) {
	SF_INFO sfInfo;
	sfInfo.samplerate = 44100;
	sfInfo.channels = 2;
	sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		
	
	sndFilePtr = sf_open(filename.c_str(), SFM_WRITE, &sfInfo);
	
		
}

void ofxSndFile::saveWithBlocks(float *left,float*right) {
	SNDFILE *sndFile = (SNDFILE*)sndFilePtr;
	int i;
	int j;
	
	for (i = 0, j = 0; i < blockLength; j+=2, i++) {
		tableBuffer[j] = left[i];
		tableBuffer[j+1] = right[i];
	}
	
	//sf_count_t res = 
	sf_writef_float(sndFile, tableBuffer, blockLength); // save buffer
	
}

void ofxSndFile::close() {
	
	
	sf_close((SNDFILE*)sndFilePtr); // release the handle to the file
	
	
}

void ofxSndFile::exit() {
	free(tableBuffer);
	tableBuffer = 0;

}
	
	

