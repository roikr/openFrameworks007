#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){
	
	
	looper.setup(256, 44100);
	for (int i=0; i<8; i++) {
		string filename = ofToDataPath("DRM_BOY_"+ofToString(i+1)+".xml");
		looper.loadLoop(filename);
	}
	
	looper.sync();
	looper.play();
	
	int midiNotes[] = {36,38,40,41,43,45,57};
	
	
	for (int i=0; i<8; i++) {
		string filename = ofToDataPath("DRM_BOY_"+ofToString(i+1)+".aif");
		instrument.loadSample(filename,midiNotes[i]);
	}
	
	instrument.setup(256);
	
	ofBackground(255,255,255);

	// 2 output channels,
	// 0 input channels
	// 22050 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)

	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	lAudio = new float[256];
	rAudio = new float[256];
	
	
	lBlock = new float[256];
	rBlock = new float[256];
	

	ofSoundStreamListDevices();
	ofSoundStreamSetup(2,0,this, sampleRate,256, 4);
	

	ofSetFrameRate(60);
	
	bpm = 120;

}


//--------------------------------------------------------------
void testApp::update(){

}

void testApp::exit() {
	
	instrument.exit();
	delete lAudio;
	delete rAudio;
	delete lBlock;
	delete rBlock;
	
}

//--------------------------------------------------------------
void testApp::draw(){


	// draw the left:
	ofSetColor(0x333333);
	ofRect(100,100,256,200);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(100+i,200,100+i,200+lAudio[i]*100.0f);
	}

	// draw the right:
	ofSetColor(0x333333);
	ofRect(600,100,256,200);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(600+i,200,600+i,200+rAudio[i]*100.0f);
	}

	ofSetColor(0x333333);
	char reportString[255];
	sprintf(reportString, "volume: (%f) modify with -/+ keys\npan: (%f)\nsynthesis: %s", volume, pan, bNoise ? "noise" : "sine wave");
	if (!bNoise) sprintf(reportString, "%s (%fhz)", reportString, targetFrequency);

	ofDrawBitmapString(reportString,80,380);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key == '-'){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+'){
		volume += 0.05;
		volume = MIN(volume, 1);
	} else if (key==' ') {
		looper.stopLoop();
	} else if (key-'0' >=0 && key-'0'<=7) {
		//instrument.noteOn(key-'0',127);
		looper.playLoop(key-'0');
	}
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
	
	int newBPM = heightPct*240;
	if (newBPM / 10 != bpm/10) {
		bpm = newBPM;
		looper.setBPM(bpm);
	}
	
	targetFrequency = 2000.0f * heightPct;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	int width = ofGetWidth();
	pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	bNoise = true;
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	bNoise = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
//--------------------------------------------------------------
void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
	//pan = 0.5f;
	float leftScale = 1 - pan;
	float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}
	
	//float left[bufferSize];
	//float right[bufferSize];
	memset(lBlock, 0, bufferSize*sizeof(float));
	memset(rBlock, 0, bufferSize*sizeof(float));
	
	vector<event> events;
	looper.process(events);
	for (vector<event>::iterator iter=events.begin(); iter!=events.end(); iter++) {
		if (iter->bNoteOn)
			instrument.noteOn(iter->note, iter->velocity);
		else 
			instrument.noteOff(iter->note);
	}
	instrument.preProcess();
	instrument.mixWithBlocks(lBlock,rBlock);
	instrument.postProcess();
	
	//sndFile2.mixWithBlocks(lBlock, rBlock);
	//sndFile2.update();
	
	//sndFile1.mixWithBlocks(lBlock, rBlock);
	
	//sndFile1.update();
	
	
	
	for (int i = 0; i < bufferSize; i++){
		lAudio[i] = output[i*nChannels] = lBlock[i] * volume * leftScale;
		rAudio[i] = output[i*nChannels + 1] =  rBlock[i] * volume *rightScale;
	}
	
			
	/*
	if ( bNoise == true){
		// ---------------------- noise --------------
		for (int i = 0; i < bufferSize; i++){
			lAudio[i] = output[i*nChannels    ] = ofRandomf() * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = ofRandomf() * volume * rightScale;
		}
	} else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for (int i = 0; i < bufferSize; i++){
			phase += phaseAdder;
			float sample = sin(phase);
			lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
		}
	}
	 
	 */

}

