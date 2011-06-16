#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	// IMPORTANT!!! if your sound doesn't work in the simulator - read this post - which requires you set the input stream to 24bit!!
	//	http://www.cocos2d-iphone.org/forum/topic/4159

	// register touch events
	ofRegisterTouchEvents(this);
	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	ofBackground(55,55,55);

	//for some reason on the iphone simulator 256 doesn't work - it comes in as 512!
	//so we do 512 - otherwise we crash
	bufferSize	= 256;
	sampleRate 			= 44100;
	drawCounter			= 0;
	bufferCounter		= 0;
	
	volume				= 0.75f;
	pan					= 0.5;
	
	int sampleLength = 1000;
	int numBuffers = sampleRate * sampleLength/ (1000 * bufferSize);
	
	sampler.setup(bufferSize,numBuffers);
	
	buffer				= new float[bufferSize];
	memset(buffer, 0, bufferSize * sizeof(float));
	
	amplitude				= new float[numBuffers];
	memset(amplitude, 0, numBuffers * sizeof(float));

	// 0 output channels,
	// 1 input channels
	// 44100 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)
	ofSoundStreamSetup(2, 1, this, sampleRate, bufferSize, 4);
	ofSetFrameRate(60);
	
	trigger.setThresh(0.1);
	compressor.setup();
	
	scale = 0.1;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	//ofTranslate(0, -50, 0);

	// draw the input:
	ofSetColor(0x333333);
	//ofRect(112,0,256,160);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < this->bufferSize; i++){
		ofLine(112+i,80,112+i,80+buffer[i]*80.0f);
	}
	
	ofPushMatrix();
	ofScale(scale, 1.0, 1.0);
	
	if (!sampler.getIsRecording()) {
		for (int i = 0; i < sampler.getNumBuffers(); i++){
			ofLine(i,320-amplitude[i]*160.0f,i,320);
		}
	}
	ofPopMatrix();
	

	ofSetColor(0x333333);
	drawCounter++;
	char reportString[255];
	sprintf(reportString, "buffers received: %i\ndraw routines called: %i\n", bufferCounter,drawCounter);
	ofDrawBitmapString(reportString, 70,10);

	trigger.draw();
}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels){
			
	if( this->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", this->bufferSize, bufferSize);
		return;
	}	
	
	double peak = 0;
	
	for (int i = 0; i < bufferSize; i++){
		buffer[i] = input[i];
		peak = max(peak,sqrt(input[i]*input[i]));
	}
	
	trigger.audioReceived(input,bufferSize);
	
	if (trigger.getTriggerState() == TRIGGER_TRIGGERED) {
		trigger.resetTrigger();
		sampler.record();
		currentBuffer = 0;
	}
	
	if ( sampler.getIsRecording() ) {
	
		sampler.audioReceived(input, bufferSize);
		
		amplitude[currentBuffer] = peak;
		currentBuffer++;
		
		if (!sampler.getIsRecording()) {
			sampler.normalize();
		}
	}
			
	// samples are "interleaved"
	
	
	//compressor.audioProcess(buffer, pbuf,bufferSize);

}


void testApp::audioRequested(float * output, int bufferSize, int nChannels){
	
	if( this->bufferSize != bufferSize ){
		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", this->bufferSize, bufferSize);
		return;
	}	
	
	float leftScale = 1 - pan;
	float rightScale = pan;
	
	if (sampler.getIsPlaying()) {
		sampler.audioRequested(0, bufferSize);
		float *input = sampler.getBuffer()+sampler.getCurrentBuffer()*sampler.getBufferSize();
		for (int i = 0; i < bufferSize; i++){
			output[i*nChannels    ] = input[i] * volume * leftScale; //lAudio[i] = 
			output[i*nChannels + 1] = input[i] * volume * rightScale; //rAudio[i] = 
		}
	} else {
		memset(output, 0, nChannels * bufferSize * sizeof(float));
	}

	
	
	
	//
//	// ---------------------- noise --------------
//	for (int i = 0; i < bufferSize; i++){
//		lAudio[i] = output[i*nChannels    ] = ofRandomf() * volume * leftScale;
//		rAudio[i] = output[i*nChannels + 1] = ofRandomf() * volume * rightScale;
//	}
//	
}


//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	if (touch.y<160) {
		switch ((int)(2*touch.x/(ofGetWidth()+1))) {
			case 0:
				trigger.setTrigger();
				trigger.resetMeters();
				break;
			case 1:
				if (!sampler.getIsRecording() ) {
					sampler.play();
				}
			default:
				break;
		}
			} else {
		lastX = touch.x;
		bDrag = true;
	}

	
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	if (bDrag) {
		scale*=touch.x/lastX;
		lastX = touch.x;
		//offset = max(0,offset);
		//offset = min(offset, bufferSize*numBuffers-ofGetWidth());
	}
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	bDrag = false;
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}
