#include "testApp.h"
#include "ofMainExt.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(127,127,127);
	
	int bufferSize = 256;
	
	
	bool bLoaded = loader.load(ofToDataPath("VOC_BOY_1.caf"));
	assert(bLoaded);
	player.setup(loader.getAudioSample(), bufferSize);
	limiter.setup(10, 500, 44100,0.3);
	//bool bLoaded = audio.load(ofToResourcesPath("alon.mov"), 256);
	
//	ofxAudioFile song;
//	song.setupForSave(256);
//	song.openForSave(ofToDocumentsPath("temp.caf"));
//	
//	
//	
//	float buffer[bufferSize*nChannels] ;
//	
//	
//	audio.play();
//	while (audio.getNumPlaying()) {
//		
//		memset(buffer, 0, bufferSize*nChannels*sizeof(float));
//		audio.mixChannel(buffer, 0, nChannels);
//		audio.mixChannel(buffer, 1, nChannels);
//		audio.postProcess();
//	
//		song.save(buffer,nChannels);
//		
//	}
//	
//	song.close();	
	
	ofSoundStreamSetup(2, 0, this, 44100, bufferSize, 2);
	
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	limiter.draw();
}

//--------------------------------------------------------------
void testApp::exit(){

}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(output, 0, bufferSize*nChannels*sizeof(float));
	player.mixChannel(output, 0, nChannels);
	player.mixChannel(output, 1, nChannels);
	player.postProcess();
	limiter.audioProcess(output, bufferSize, nChannels);
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	sampleInstance si;
	si.speed = 1+touch.y/ofGetHeight();
	si.left = 1.0f;
	si.right = 1.0f;
	si.retrigger = false;
	
	player.trigger(si);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}

