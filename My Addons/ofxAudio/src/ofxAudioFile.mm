/*
 *  ofxAudioFile.cpp
 *  audioOutputExample
 *
 *  Created by Roee Kremer on 7/13/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxAudioFile.h"
//#import <AVFoundation/AVFoundation.h>

#include "CAStreamBasicDescription.h"
#include "CAXException.h"

#define SINT16_MAX 32767.0
#define LOADING_BUFFER_SIZE 8192

//#define LOG_AUDIO_FILE

#include <iostream>

ofxAudioFile::ofxAudioFile() {
	saveBuffer = NULL;
	tableBuffer = NULL;
	bLoaded = false;
}


bool ofxAudioFile::load(string filename,int bufferLength) {
	
	OSStatus        error = noErr;

	//bIsPlaying = false;
	bLoaded = false;
	this->bufferLength = bufferLength;	
	
	this->filename = filename;
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
		channels = format.NumberChannels();
		
		
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
		
		samplesPerChannel = frames;
		tableBuffer = (float*) malloc(frames * format.NumberChannels() * sizeof(float));
		
		SInt16 *samples = (SInt16*)srcBuffer;
		
		int readPos = 0;
		while (readPos<frames) {
			UInt32 numFrames = LOADING_BUFFER_SIZE;
			XThrowIfError(ExtAudioFileRead(file, &numFrames, &fillBufList), "ExtAudioFileRead failed!");	
						
			for (int i=0; i<numFrames; i++) {
				SInt16 sample = samples[i*channels];
				tableBuffer[readPos+i] = (float)sample / SINT16_MAX;
			}
			
			if (channels==2) {
				for (int i=0; i<numFrames; i++) {
					SInt16 sample = samples[i*channels+1];
					tableBuffer[samplesPerChannel+readPos+i] = (float)sample / SINT16_MAX;
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

/*
bool ofxAudioFile::load(string filename,int blockLength) {
	
	
	bIsPlaying = false;
	this->blockLength = blockLength;	
	
	NSString *path = [NSString stringWithFormat:@"%s",filename.c_str()];
	AVURLAsset *asset = [AVURLAsset URLAssetWithURL:[NSURL fileURLWithPath:path] options:nil];
	
	NSError *error = nil;
	
	AVAssetReader *reader = [AVAssetReader assetReaderWithAsset:asset error:&error];
	
	if (error) {
		NSLog(@"AVAssetReader: %@",[error description]);
	}
	
	NSArray *audioTracks = [asset tracksWithMediaType:AVMediaTypeAudio];
	
	NSDictionary* audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM ], AVFormatIDKey,
										 [NSNumber numberWithBool:YES], AVLinearPCMIsFloatKey,
										 [NSNumber numberWithInteger:32], AVLinearPCMBitDepthKey,
										[NSNumber numberWithBool:NO], AVLinearPCMIsBigEndianKey,
//										[NSNumber numberWithBool:YES], AVLinearPCMIsNonInterleaved,
										 //	[NSNumber numberWithFloat:video->sampleRate],AVSampleRateKey,
										 nil];
	
	AVAssetReaderTrackOutput * audioOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[audioTracks objectAtIndex:0] outputSettings:audioOutputSettings];
	NSLog(@"AVAssetReaderTrackOutput mediaType: %@",[audioOutput mediaType]);
	
	 
	BOOL test = [reader canAddOutput:audioOutput];
	NSLog(@"can add output: %i",test);
	[reader addOutput:audioOutput];
	
	[reader startReading];
	
	
	int bufferLength = 0;
	
	while (reader.status == AVAssetReaderStatusReading) {
		
		
		CMSampleBufferRef  sampleBuffer = [audioOutput copyNextSampleBuffer];
		if (sampleBuffer != NULL) {
			CMItemCount numSamples = CMSampleBufferGetNumSamples(sampleBuffer);
			NSUInteger channelIndex = 0;
			
			if (!bufferLength) {
				CMFormatDescriptionRef formarDesc = CMSampleBufferGetFormatDescription (sampleBuffer);
				const AudioStreamBasicDescription * streamDesc = CMAudioFormatDescriptionGetStreamBasicDescription (formarDesc);
				channels = streamDesc->mChannelsPerFrame;
				
				tableBuffer = (float *) malloc(numSamples*channels * sizeof(float));
			}
			
			CMBlockBufferRef audioBlockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
			size_t audioBlockBufferOffset = (channelIndex * numSamples * sizeof(SInt16));
			size_t lengthAtOffset = 0;
			size_t totalLength = 0;
			Float32 *samples = NULL;
			CMBlockBufferGetDataPointer(audioBlockBuffer, audioBlockBufferOffset, &lengthAtOffset, &totalLength, (char **)(&samples));
			NSLog(@"numSamples: %i, audioBlockBufferOffset: %i, lengthAtOffset: %i, totalLength %i",numSamples,audioBlockBufferOffset,lengthAtOffset,totalLength);
			
			
			
			memcpy(tableBuffer, samples, numSamples*channels * sizeof(float));
			
			bufferLength+=numSamples*channels;
			

			
			CFRelease(sampleBuffer);
		}
		
	}
	
	NSLog(@"status: %i",reader.status);
	
	if (reader.status == AVAssetReaderStatusFailed) {
		NSLog(@"error: %@",[reader.error description]);
	}
	
	
	
	samplesPerChannel =bufferLength/channels;
	
	//tableBuffer = new float[bufferLength * sizeof(float)];
	
	
	NSLog(@"bufferLength: %i",bufferLength);
	
	
	
	
	
		
	return true;
}	
 
 */

void ofxAudioFile::setupForSave(int bufferLength) {
	this->bufferLength = bufferLength;
	this->samplesPerChannel = 0;
	channels = 2;
	
	saveBuffer = (SInt16*) malloc(bufferLength * channels * sizeof(SInt16));
	fillBufList.mNumberBuffers = 1;
	fillBufList.mBuffers[0].mNumberChannels = channels;
	fillBufList.mBuffers[0].mDataByteSize = bufferLength*sizeof(SInt16);
	fillBufList.mBuffers[0].mData = saveBuffer;
}

float *ofxAudioFile::getTableBuffer() {
	return tableBuffer;
}

int ofxAudioFile::getSamplesPerChannel() {
	return samplesPerChannel;
}

/*
void ofxAudioFile::play(float speed) {
	currentBlock = 0;
	this->speed = speed;
	bIsPlaying = true;
}
 */

void ofxAudioFile::play() {
	trigger(1.0f, 1.0f, true);
}

void ofxAudioFile::trigger(float speed,float volume,bool retrigger) {
	if (retrigger && !instances.empty()) {
		instances.back().bStop = true;
#ifdef LOG_AUDIO_FILE
		cout << "retrigger, ";
#endif
	}
	instance i;
	i.pos = 0;
	i.volume = volume;
	i.bStop = false;
	i.speed = speed;
	instances.push_front(i);
#ifdef LOG_AUDIO_FILE
	cout << "trigger: " << instances.size() << endl; // ", blocks: " << sample.getSamplesPerChannel()/blockLength << endl;
#endif
	
}

void ofxAudioFile::stop() {
	for (deque<instance >::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		iter->bStop = true;
	}
}



/*
void ofxAudioFile::mixWithBlocks(float *left,float *right,float volume) {
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
*/

/*
void ofxAudioFile::channelRequested(float * output, int channel, int nChannels,float volume) {
	if (!bIsPlaying ) {
		return;
	} else {
		
		int n = getIsLastBlock() ? (currentBlock+1) * blockLength - samplesPerChannel : blockLength;
		
		
		
		
		float *buffer = channels == 2 ? getCurrentBlock(channel) : getCurrentBlock(0);
		
		for (int i=0; i<n; i++) {
			output[i*nChannels+channel]+=buffer[i]*volume;
		}
		
	}
}
*/

/*
void ofxAudioFile::mix(float *left,float *right,int block,float volume,bool ramp) {

	int n = blockLength;
	if (getIsLastBlock(block)) {
	  n =   (block+1) * blockLength - samplesPerChannel ;
	}
	
	float *channel0 = getBlock(block,0);
	float *channel1 = channels == 2 ? getBlock(block,1) : channel0;
	
	if (ramp) {
		float step = 1.0/(n-1);
#ifdef LOG_AUDIO_FILE
		cout << "ramp" << endl; //<< step << endl;
#endif
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
*/

void ofxAudioFile::mixChannel(float * output, int channel, int nChannels) {
	
	
	for (deque<instance>::iterator iter=instances.begin(); iter!=instances.end(); iter++) {
		//sample.mixChannel(output,channel,nChannels,iter->block,iter->volume,iter->bStop);
	
	
		int n = bufferLength;
		if (iter->pos+bufferLength*iter->speed>=samplesPerChannel) {
			//n =   (block+1) * blockLength - samplesPerChannel ;
			n = (samplesPerChannel - iter->pos)/iter->speed;
		}
		
		//return (tableBuffer+samplesPerChannel*channel+block*blockLength);
		float *buffer = tableBuffer+samplesPerChannel*(channels == 2 ? channel : 0)+iter->pos;
		
		if (iter->bStop) {
			float step = 1.0/(n-1);
#ifdef LOG_AUDIO_FILE
			cout << "ramp" << endl; //<< step << endl;
#endif
			for (int i=0; i<n; i++) {
				output[i*nChannels+channel]+=buffer[(int)(iter->speed*i)]*iter->volume*((n-1-i)*step);
			}
		} else {
			for (int i=0; i<n; i++) {
				output[i*nChannels+channel]+=buffer[(int)(iter->speed*i)]*iter->volume;
			}
		}
	}
}


/*
void ofxAudioFile::audioRequested (float * output, int channel,int bufferSize, int nChannels) {
	if (!bIsPlaying ) {
		return;
	} else {
		
		int n = getIsLastBlock() ? (currentBlock+1) * blockLength - samplesPerChannel : blockLength;
		
		
		
		
		float *buffer = channels == 2 ? getCurrentBlock(channel) : getCurrentBlock(0);
		
		for (int i=0; i<n; i++) {
			output[i*nChannels+channel]=buffer[i];
		}
		
		for (int i=n; i<bufferSize; i++) {
			output[i*nChannels+channel]=0;
		}
		
		
	}
	
}

*/

void ofxAudioFile::postProcess() {
	deque<instance>::iterator iter=instances.begin();
	while (iter!=instances.end()) {
		if (iter->pos+bufferLength*iter->speed>=samplesPerChannel || iter->bStop) {
			iter = instances.erase(iter);
#ifdef LOG_AUDIO_FILE
			cout << "done" << endl; //<< step << endl;
#endif
		} else {
			iter->pos+=bufferLength*iter->speed;
			iter++;
		}
	}
}

int ofxAudioFile::getNumPlaying() {
	return instances.size();
}

void ofxAudioFile::openForSave(string filename) {
	
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

void ofxAudioFile::save(float * output,int nChannels) {

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

	
void ofxAudioFile::close() {
	
	
	ExtAudioFileDispose(file);
	file = 0;
	
	
}

void ofxAudioFile::exit() {
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





/*
void ofxAudioFile::open(string filename) {
	
	//SF_INFO sfInfo;
//	sfInfo.samplerate = 44100;
//	sfInfo.channels = 2;
//	sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
//		
//	
//	sndFilePtr = sf_open(filename.c_str(), SFM_WRITE, &sfInfo);

	
	NSError *error = nil;
	
	AudioChannelLayout layout;
	layout.mChannelLayoutTag = kAudioChannelLayoutTag_Stereo   ;
	
	NSDictionary* audioSettings = [NSDictionary dictionaryWithObjectsAndKeys:
								   [NSNumber numberWithInt: kAudioFormatLinearPCM ],AVFormatIDKey, // // kAudioFormatMPEG4AAC
								   [NSNumber numberWithFloat: 44100. ],AVSampleRateKey,
								   [NSNumber numberWithInt: 2 ],AVNumberOfChannelsKey,
								   [NSData dataWithBytes:&layout length:sizeof(AudioChannelLayout)],AVChannelLayoutKey,
								   nil];
								   
										 
    //keys[3] = AVEncoderBitRateKey; values[3] = [NSNumber numberWithInt: 192000 ]; //[NSNumber numberWithInt: 64000 ];
    	
	audioInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:audioSettings];
	audioInput.expectsMediaDataInRealTime = NO;
	
	writer = [AVAssetWriter assetWriterWithURL:[NSString stringWithFormat:@"%s",filename.c_str()] fileType:AVFileTypeCoreAudioFormat  error:&error]; // kUTTypeMPEG4
	if (error) {
		NSLog(@"AVAssetWriter: %@",[error description]);
	}
	
	
	BOOL test = [writer canAddInput:audioInput];
	NSLog(@"can add audioInput: %i",test);
	[writer addInput:audioInput];
	
	
	[writer startWriting];
	[writer startSessionAtSourceTime:kCMTimeZero] ;//CMTimeAdd(kCMTimeZero, CMTimeMultiply(CMTimeMake(1, 25), frameNum))];
	NSLog(@"Writing Started");
			
	
}
 

void ofxAudioFile::saveWithBlocks(float *left,float*right) {
	//SNDFILE *sndFile = (SNDFILE*)sndFilePtr;
//	int i;
//	int j;
//	
//	for (i = 0, j = 0; i < blockLength; j+=2, i++) {
//		tableBuffer[j] = left[i];
//		tableBuffer[j+1] = right[i];
//	}
//	
//	//sf_count_t res = 
//	sf_writef_float(sndFile, tableBuffer, blockLength); // save buffer
	
	
	while (![audioInput isReadyForMoreMediaData]);
		
		NSLog(@"writing audio");
		CMSampleBufferRef nextSampleBuffer 			[audioInput appendSampleBuffer:nextSampleBuffer];
			CFRelease(nextSampleBuffer);
		}
		else
		{
			
			bAudioFinished = true;
		}
		
	
	
}

void ofxAudioFile::close() {
	
	//[writer endSessionAtSourceTime:CMTimeAdd(kCMTimeZero, CMTimeMultiply(CMTimeMake(1, 25), frameNum-1))];
	[audioInput markAsFinished];
	[writer finishWriting];
	NSLog(@"Writing finished with status: %i",[writer status]);
	
	
//	sf_close((SNDFILE*)sndFilePtr); // release the handle to the file
	
	
}
 */



	
	

