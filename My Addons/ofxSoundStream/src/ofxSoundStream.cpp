#include "ofxSoundStream.h"
#include "RtAudio.h"

//----------------------------------- static variables:
static ofBaseApp 	* 		OFSAptrX = NULL;
RtAudio				*		audioX = NULL;
int 						nInputChannelsX;
int 						nOutputChannelsX;
ofAudioEventArgs 			audioEventArgsX;


int receiveAudioBufferAndCallSimpleAppX(void *outputBuffer, void *inputBuffer, unsigned int bufferSize,
           double streamTime, RtAudioStreamStatus status, void *data);


//------------------------------------------------------------------------------
int receiveAudioBufferAndCallSimpleAppX(void *outputBuffer, void *inputBuffer, unsigned int bufferSize,
           double streamTime, RtAudioStreamStatus status, void *data){


	if ( status ) std::cout << "Stream over/underflow detected." << std::endl;

	// 	rtAudio uses a system by which the audio
	// 	can be of different formats
	// 	char, float, etc.
	// 	we choose float
	float * fPtrOut = (float *)outputBuffer;
	float * fPtrIn = (float *)inputBuffer;
	// [zach] memset output to zero before output call
	// this is because of how rtAudio works: duplex w/ one callback
	// you need to cut in the middle. if the simpleApp
	// doesn't produce audio, we pass silence instead of duplex...


	if (nInputChannelsX > 0){
		if(OFSAptrX) OFSAptrX->audioReceived(fPtrIn, bufferSize, nInputChannelsX);
		#ifdef OF_USING_POCO
			audioEventArgsX.buffer = fPtrIn;
			audioEventArgsX.bufferSize = bufferSize;
			audioEventArgsX.nChannels = nInputChannelsX;
			ofNotifyEvent( ofEvents.audioReceived, audioEventArgsX );
		#endif
		memset(fPtrIn, 0, bufferSize * nInputChannelsX * sizeof(float));
	}


	if (nOutputChannelsX > 0) {
		if(OFSAptrX) OFSAptrX->audioRequested(fPtrOut, bufferSize, nOutputChannelsX);
		#ifdef OF_USING_POCO
			audioEventArgsX.buffer = fPtrOut;
			audioEventArgsX.bufferSize = bufferSize;
			audioEventArgsX.nChannels = nOutputChannelsX;
			ofNotifyEvent( ofEvents.audioRequested, audioEventArgsX );
		#endif
	}

	return 0;
}
//---------------------------------------------------------
void ofxSoundStreamSetup(int deviceId,int nOutputs, int nInputs, ofBaseApp * OFSA){
	ofxSoundStreamSetup(deviceId,nOutputs, nInputs, OFSA, 44100, 256, 4);
}

//---------------------------------------------------------
void ofxSoundStreamSetup(int deviceId,int nOutputs, int nInputs, int sampleRate, int bufferSize, int nBuffers){
	ofxSoundStreamSetup(deviceId,nOutputs, nInputs, NULL, sampleRate, bufferSize, nBuffers);
}

//---------------------------------------------------------
void ofxSoundStreamSetup(int deviceId,int nOutputs, int nInputs, ofBaseApp * OFSA, int sampleRate, int bufferSize, int nBuffers){

	nInputChannelsX 		=  nInputs;
	nOutputChannelsX 	=  nOutputs;
	OFSAptrX 			=  OFSA;
	bufferSize = ofNextPow2(bufferSize);	// must be pow2


	try {
		audioX = new RtAudio();
	}	catch (RtError &error) {
		error.printMessage();
		//std::exit(EXIT_FAILURE); // need case here
	}


	RtAudio::StreamParameters * outputParameters=NULL;
	if(nOutputChannelsX >0){
		outputParameters = new RtAudio::StreamParameters();
		outputParameters->deviceId = deviceId;//audio->getDefaultOutputDevice();
		outputParameters->nChannels = nOutputChannelsX;
	}

	RtAudio::StreamParameters * inputParameters = NULL;
	if(nInputChannelsX>0){
		inputParameters = new RtAudio::StreamParameters;
		inputParameters->deviceId = audioX->getDefaultInputDevice();
		inputParameters->nChannels = nInputChannelsX;
	}

	unsigned int bufferFrames = (unsigned int)bufferSize; // 256 sample frames

	RtAudio::StreamOptions options;
	options.flags = RTAUDIO_SCHEDULE_REALTIME;
	options.numberOfBuffers = nBuffers;
	options.priority = 1;

	try {

		audioX ->openStream( outputParameters, inputParameters, RTAUDIO_FLOAT32,
							sampleRate, &bufferFrames, &receiveAudioBufferAndCallSimpleAppX, &options);
		audioX->startStream();
	} catch (RtError &error) {
		error.printMessage();
	}

}

//---------------------------------------------------------
void ofxSoundStreamStop(){
	try {
    	audioX->stopStream();
  	} catch (RtError &error) {
   		error.printMessage();
 	}
}



//---------------------------------------------------------
void ofxSoundStreamStart(){
	try{
		audioX->startStream();
	} catch (RtError &error) {
		error.printMessage();
	}
}


//---------------------------------------------------------
void ofxSoundStreamClose(){
	if(!audioX) return;
	try {
    	audioX->stopStream();
    	audioX->closeStream();
  	} catch (RtError &error) {
   		error.printMessage();
 	}
	delete audioX;
}


//---------------------------------------------------------
void ofxSoundStreamListDevices(){
	RtAudio *audioTemp = 0;
	try {
		audioTemp = new RtAudio();
	} catch (RtError &error) {
		error.printMessage();
	}
 	int devices = audioTemp->getDeviceCount();
	RtAudio::DeviceInfo info;
	for (int i=0; i< devices; i++) {
		try {
			info = audioTemp->getDeviceInfo(i);
		} catch (RtError &error) {
			error.printMessage();
			break;
		}
		std::cout << "device = " << i << " (" << info.name << ")\n";
		if (info.isDefaultInput) std::cout << "----* default ----* \n";
		std::cout << "maximum output channels = " << info.outputChannels << "\n";
		std::cout << "maximum input channels = " << info.inputChannels << "\n";
		std::cout << "-----------------------------------------\n";

	}
	delete audioTemp;
}
