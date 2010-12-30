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
	
	ofBackground(0,0,0);
	
	ofSetFrameRate(60);
	
	
	video.bufferSize = 512;
	video.fps = 25;
	video.numIntroFrames = 0;
	video.sampleRate 			= 44100;
		
	video.widthFraction = 1.0;
	video.heightFraction = 1.0;
	video.textureWidth = 480;
	video.textureHeight = 360;
	
	
	loader.setup(&video);
	loader.load(ofToDataPath("orenv.mov"));
	video.bHorizontal = false;
	player.setup(&video, false);
	
	lAudio	= new float[video.audio.getBufferSize()];
	rAudio	= new float[video.audio.getBufferSize()];
	
	ofSoundStreamSetup(2, 0, this, video.sampleRate, video.bufferSize, 2);
	
}

//--------------------------------------------------------------
void testApp::update(){
	player.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofPushMatrix();
	ofScale(0.5, 0.5, 1);
	if (player.getIsPlaying()) {
		player.draw();
	} 
	
	ofPopMatrix();
	
	ofSetColor(255, 255, 255);
	ofFill();
	ofRect(0, 0, 20, 20);
	
}

//--------------------------------------------------------------
void testApp::exit(){
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	float note = 0.5 + touch.x / (ofGetWidth());
	player.play( note,1.0f );
	//player.play(60,127);
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


void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(lAudio, 0, bufferSize*sizeof(float));
	memset(rAudio, 0, bufferSize*sizeof(float));
	
	float pan = 0.5;
	
	float leftScale = 1 - pan;
	float rightScale = pan;
	
	player.mix(lAudio, bufferSize,leftScale);
	player.mix(rAudio, bufferSize,rightScale);
	player.preProcess();
	
	for (int i = 0; i < bufferSize; i++){
		output[i*nChannels] = lAudio[i];// * gain;
		output[i*nChannels + 1] = rAudio[i];// * gain;
	}
	
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

