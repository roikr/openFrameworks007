#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	
		
	instrument.setup(256,2);
	
	for (int i=48; i<=72; i++) {
		string filename = ofToDataPath("samples/Littlevoice_"+ofToString(i)+".caf");
		instrument.loadSample(filename,i);
	}
	
	
	
	// IMPORTANT!!! if your sound doesn't work in the simulator - read this post - which requires you set the output stream to 24bit 
	//	http://www.cocos2d-iphone.org/forum/topic/4159

	// register touch events
	ofRegisterTouchEvents(this);
	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	ofBackground(255,255,255);

	// 2 output channels,
	// 0 input channels
	// 44100 samples per second
	// 512 samples per buffer
	// 4 num buffers (latency)

	sampleRate 			= 44100;
	
	volume				= 0.8f;
	pan					= 0.5;
	
	
	//for some reason on the iphone simulator 256 doesn't work - it comes in as 512!
	//so we do 512 - otherwise we crash
	
	
	lAudio				= new float[256];
	rAudio				= new float[256];
	
	lBlock = new float[256];
	rBlock = new float[256];
	
	memset(lAudio, 0, 256 * sizeof(float));
	memset(rAudio, 0, 256 * sizeof(float));
	
	//we do this because we don't have a mouse move function to work with:
	//targetFrequency = 444.0;
	//phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
	
	ofSoundStreamSetup(2,0,this, sampleRate, 256, 4);
	ofSetFrameRate(60);
	
	

}

void testApp::exit() {
	
	instrument.exit();
	delete lAudio;
	delete rAudio;
	delete lBlock;
	delete rBlock;
	
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){

	float boxW		= 200.0;
	float boxH		= boxW*0.75;
	
	float topY		= 30;
	float leftX		= 30;
	float rightX	= leftX + boxW + 20;

	// draw the left:
	ofSetColor(0x333333);
	ofRect(leftX,topY,boxW,boxH);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		float x = ofMap(i, 0, 256, 0, boxW, true);
		ofLine(leftX+x,topY + boxH/2,leftX+x,topY + boxH/2 + lAudio[i]*boxH*0.5);
	}

	// draw the right:
	ofSetColor(0x333333);
	ofRect(rightX,topY,boxW,boxH);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		float x = ofMap(i, 0, 256, 0, boxW, true);	
		ofLine(rightX+x,topY + boxH/2,rightX+x,topY + boxH/2 + rAudio[i]*boxH*0.5);
	}

	ofSetColor(0x333333);
	char reportString[255];
	sprintf(reportString, "volume: (%f) \npan: (%f", volume, pan);
	

	ofDrawBitmapString(reportString,leftX,topY + boxH + 20);

}

//--------------------------------------------------------------
void testApp::audioRequested(float * output, int bufferSize, int nChannels){
		
	//if( initialBufferSize != bufferSize ){
//		ofLog(OF_LOG_ERROR, "your buffer size was set to %i - but the stream needs a buffer size of %i", initialBufferSize, bufferSize);
//		return;
//	}	
	//pan = 0.5f;
	float leftScale = 1 - pan;
	float rightScale = pan;

	memset(lBlock, 0, bufferSize*sizeof(float));
	memset(rBlock, 0, bufferSize*sizeof(float));
	
	
	
	
	instrument.preProcess();
	//instrument.mixWithBlocks(lBlock,rBlock);
	instrument.mixChannel(lBlock, 0, 1);
	instrument.mixChannel(rBlock, 0, 1);
	instrument.postProcess();
	
	for (int i = 0; i < bufferSize; i++){
		lAudio[i] = output[i*nChannels] = lBlock[i] * volume * leftScale;
		rAudio[i] = output[i*nChannels + 1] =  rBlock[i] * volume *rightScale;
	}
	

	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
		
	lastKey = 48+(int)(touch.y/20) ;
	lastArp = (int)((ofGetWidth()-touch.x)/120) ;
	
	instrument.noteOn(lastKey+lastArp*4, 127);
			 
	//instrument.noteOff(iter->note);
			
		
	
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	
	int key = 48+(int)(touch.y/20) ;
	int arp = (int)((ofGetWidth()-touch.x)/120) ;
	if (key!=lastKey || arp!=lastArp) {
		lastKey = key;
		lastArp = arp;
		instrument.noteOn(lastKey+lastArp*4, 127);
		
	}
	
//		int width = ofGetWidth();
//		pan = (float) touch.x / (float)width;
		
//	float height = (float)ofGetHeight();
//	float heightPct = ((height-touch.y) / height);
		
		
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
}

