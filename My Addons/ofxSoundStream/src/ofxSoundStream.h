#ifndef _OFX_SOUND_STREAM
#define _OFX_SOUND_STREAM

#include "ofConstants.h"
#include "ofBaseApp.h"
#include "ofEvents.h"
#include "ofMath.h"

void ofxSoundStreamSetup(int deviceId, int nOutputChannels, int nInputChannels, ofBaseApp * OFSA = NULL);
void ofxSoundStreamSetup(int deviceId, int nOutputChannels, int nInputChannels, int sampleRate, int bufferSize, int nBuffers);
void ofxSoundStreamSetup(int deviceId, int nOutputChannels, int nInputChannels, ofBaseApp * OFSA, int sampleRate, int bufferSize, int nBuffers);
void ofxSoundStreamStop();
void ofxSoundStreamStart();
void ofxSoundStreamClose();
void ofxSoundStreamListDevices();

#endif
