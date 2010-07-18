/*
 *  ofxSndFile.cpp
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSndFile.h"

#include <sndfile.h>
#include <iostream>

bool ofxSndFile::load(string filename) {
	
	
	
		   
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
	
	
	sf_count_t res = sf_read_float(sndFile, buffer, bufferLength); // read the whole file into memory
	sf_close(sndFile); // release the handle to the file
	
	//for (int k=0; k<samplesPerChannel; k++) {
//		printf("%0.8f\t", buffer[k]);
//	}
	
	channels = sfInfo.channels;
	
	switch (channels) {
		case 1:
			tableBuffer = buffer;
			break;
		case 2:
			tableBuffer = (float *) malloc(bufferLength * sizeof(float));
			for (int i = 0, j = 0; i < samplesPerChannel; i+=sfInfo.channels, j++) {
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

float *ofxSndFile::getTableBuffer() {
	return tableBuffer;
}

int ofxSndFile::getSamplesPerChannel() {
	return samplesPerChannel;
}

void ofxSndFile::setup(int blockLength) {
	this->blockLength = blockLength;
	bIsPlaying = false;
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

float* ofxSndFile::getCurrentBlock(int channel) {
	return (tableBuffer+samplesPerChannel*channel+currentBlock*blockLength);
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

void ofxSndFile::postProcess() {
	if (!bIsPlaying)
		return;
	
	
	if (getIsLastBlock()) {
		bIsPlaying = false;
	}
	else
		currentBlock++;
	
}

void ofxSndFile::exit() {
	free(tableBuffer);
}
	
	

