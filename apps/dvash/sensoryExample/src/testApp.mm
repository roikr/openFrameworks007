#include "testApp.h"


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
    
    for (vector<ofxAudioFile>::iterator iter = sounds.begin(); iter!=sounds.end(); iter++) {
        <#statements#>
    }
	
	bool bLoaded = audio.load(ofToResourcesPath("VOC_PLASTIC_1.caf"), bufferSize);
	//bool bLoaded = audio.load(ofToResourcesPath("alon.mov"), 256);
	
	ofxAudioFile song;
	song.setupForSave(256);
	
	cout << "renderAudio started" << endl;
	
	song.open(ofToDocumentsPath("temp.caf"));
	
	
	
	float lBlock[bufferSize] ;
	float rBlock[bufferSize];
	
	audio.play();
	while (audio.getIsPlaying()) {
		
		memset(lBlock, 0, bufferSize*sizeof(float));
		memset(rBlock, 0, bufferSize*sizeof(float));
		audio.mixWithBlocks(lBlock, rBlock, 1.0);
		
		audio.postProcess();
	
		song.saveWithBlocks(lBlock, rBlock);
		
	}
	
	song.close();	
	
	ofSoundStreamSetup(2, 0, this, 44100, bufferSize, 2);
	
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::exit(){

}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	audio.audioRequested(output, 0, bufferSize, nChannels);	
	audio.audioRequested(output, 1, bufferSize, nChannels);	
	audio.postProcess();
	
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	audio.play();
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

