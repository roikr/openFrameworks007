/*
 *  ofxAudioLoader.cpp
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxAudioLoader.h"


#include "CAStreamBasicDescription.h"
#include "CAXException.h"

#define SINT16_MAX 32767.0
#define LOADING_BUFFER_SIZE 8192

//#define LOG_AUDIO_FILE

#include <iostream>

//ofxAudioLoader::ofxAudioLoader() {
//	saveBuffer = NULL;
//	tableBuffer = NULL;
//	bLoaded = false;
//}


bool ofxAudioLoader::load(string filename) {
	
	OSStatus        error = noErr;

	//bIsPlaying = false;
	bLoaded = false;
	
	NSString *path = [NSString stringWithFormat:@"%s",filename.c_str()];
	
    CFURLRef pathURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)path, kCFURLPOSIXPathStyle, false);
	
	try { 
				ExtAudioFileRef file = 0;
		
		XThrowIfError(ExtAudioFileOpenURL(pathURL, &file), "ExtAudioFileOpenURL failed");
		
        // get the source data format
		CAStreamBasicDescription format;
		UInt32 size = sizeof(format);
		XThrowIfError(ExtAudioFileGetProperty(file, kExtAudioFileProperty_FileDataFormat, &size, &format), "couldn't get source data format");
		//printf("Source file format: "); 
		//format.Print();
		
		//printf("is interleaved: %i\n",format.IsInterleaved()); 
		audio.nChannels = format.NumberChannels();
		
		
		SInt64 frames;
		size = sizeof(frames);
		XThrowIfError(ExtAudioFileGetProperty(file, kExtAudioFileProperty_FileLengthFrames, &size, &frames), "couldn't get source length frames");
		//printf("Source file length frames: %i\n",(int)frames); 
		
		
		//UInt32 bufferByteSize  = format.FramesToBytes(frames);
		//printf("frame to bytes: %i\n",(int)bufferByteSize);
		//cout<< "loading audio file: " << filename <<  ", length: " << bufferByteSize << "\n";
		
        char srcBuffer[LOADING_BUFFER_SIZE];
		
		AudioBufferList fillBufList;
		fillBufList.mNumberBuffers = 1;
		fillBufList.mBuffers[0].mNumberChannels = format.NumberChannels();
		fillBufList.mBuffers[0].mDataByteSize = LOADING_BUFFER_SIZE;
		fillBufList.mBuffers[0].mData = srcBuffer;
		
		audio.numFrames = frames;
		audio.buffer = (float*) malloc(frames * format.NumberChannels() * sizeof(float));
		
		SInt16 *samples = (SInt16*)srcBuffer;
		
		int readPos = 0;
		while (readPos<frames) {
			UInt32 numFrames = LOADING_BUFFER_SIZE;
			XThrowIfError(ExtAudioFileRead(file, &numFrames, &fillBufList), "ExtAudioFileRead failed!");	
						
			for (int i=0; i<numFrames; i++) {
				SInt16 sample = samples[i*audio.nChannels];
				audio.buffer[readPos+i] = (float)sample / SINT16_MAX;
			}
			
			if (audio.nChannels==2) {
				for (int i=0; i<numFrames; i++) {
					SInt16 sample = samples[i*audio.nChannels+1];
					audio.buffer[audio.numFrames+readPos+i] = (float)sample / SINT16_MAX;
				}
			}
			
			readPos+=numFrames;

			
		}
		
		XThrowIfError(ExtAudioFileDispose(file), "ExtAudioFileDispose failed!");
		bLoaded = true;
	}		
		
	catch (CAXException e) {
		char buf[256];
		fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
        error = e.mError;
	}	
	
	
	
	return bLoaded;
}	

ofxAudioSample* ofxAudioLoader::getAudioSample() {return &audio;}


/*
void ofxAudioLoader::setupForSave(int bufferLength) {
	this->bufferLength = bufferLength;
	this->samplesPerChannel = 0;
	channels = 2;
	
	saveBuffer = (SInt16*) malloc(bufferLength * channels * sizeof(SInt16));
	fillBufList.mNumberBuffers = 1;
	fillBufList.mBuffers[0].mNumberChannels = channels;
	fillBufList.mBuffers[0].mDataByteSize = bufferLength*sizeof(SInt16);
	fillBufList.mBuffers[0].mData = saveBuffer;
}

float *ofxAudioLoader::getTableBuffer() {
	return tableBuffer;
}









void ofxAudioLoader::openForSave(string filename) {
	
	this->filename = filename;
	OSStatus error = noErr;
	NSString *path = [NSString stringWithFormat:@"%s",filename.c_str()];
	
    CFURLRef pathURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)path, kCFURLPOSIXPathStyle, false);
	
	try { 
		//ExtAudioFileRef file = 0;
		
		// PCM is the destination format, create a 16-bit int PCM file format description
		CAStreamBasicDescription format;
		format.mSampleRate = 44100; // set sample rate
		format.mFormatID = kAudioFormatLinearPCM;
		format.mChannelsPerFrame = channels;
		format.mBitsPerChannel = 16;
		format.mBytesPerPacket = format.mBytesPerFrame = channels * format.mChannelsPerFrame;
		format.mFramesPerPacket = 1;
		format.mFormatFlags = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger;  
		
		XThrowIfError(ExtAudioFileCreateWithURL(pathURL, kAudioFileCAFType, &format, NULL, kAudioFileFlags_EraseFile, &file), "ExtAudioFileCreateWithURL failed!");
	}
	catch (CAXException e) {
		char buf[256];
		fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
        error = e.mError;
	}
	
	
	
	
	
}

void ofxAudioLoader::save(float * output,int nChannels) {

	OSStatus error = noErr;

	
	int i;
	int j;
	
	for (i = 0, j = 0; i < bufferLength; j+=2, i++) {
		saveBuffer[j] = (float)(output[i*nChannels]*SINT16_MAX);
		saveBuffer[j+1] = (float)(output[(i+1)*nChannels-1]*SINT16_MAX);
	}
	
	try {
		XThrowIfError(ExtAudioFileWrite(file, bufferLength, &fillBufList), "ExtAudioFileWrite failed!");
	}
		
	catch (CAXException e) {
		char buf[256];
		fprintf(stderr, "Error: %s (%s)\n", e.mOperation, e.FormatError(buf));
        error = e.mError;
	}	
	
	
	
}	

	
void ofxAudioLoader::close() {
	
	
	ExtAudioFileDispose(file);
	file = 0;
	
	
}

void ofxAudioLoader::exit() {
	if (tableBuffer) {
		free(tableBuffer);
		tableBuffer = 0;
		
#ifdef LOG_AUDIO_FILE
		cout << "free tableBuffer: " << filename << endl; //<< step << endl;
#endif
		
	}
	
	if (saveBuffer) {
		free(saveBuffer);
		saveBuffer = 0;
#ifdef LOG_AUDIO_FILE
		cout << "free saveBuffer: " << filename << endl; //<< step << endl;
#endif
	}
	
}
*/



	
	

