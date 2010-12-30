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
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(0,0,0);
	
	ofSetFrameRate(60);
	
	
	int bufferSize = 512;
	
	video.fps = 25;
	video.numIntroFrames = 4;
	video.sampleRate 			= 44100;
	video.numFrames = 25;
	video.sampleLength = 1000*video.numFrames/video.fps;
	
	video.audio.setup(bufferSize,video.sampleRate * video.sampleLength / (1000 * bufferSize));	
	video.widthFraction = 1.0;
	video.heightFraction = 1.0;
	video.textureWidth = 128;
	video.textureHeight = 256;
	
	
	camera.setup(&video);
	player.setup(&video, false);
	
	camera.startCamera();
}

//--------------------------------------------------------------
void testApp::update(){
	player.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	//camera->fboDraw();
	camera.render();
	
	if (player.getIsPlaying()) {
		player.draw();
	} else {
		camera.draw();
	}
	
//	ofSetColor(255, 255, 255);
//	ofFill();
//	ofRect(0, 0, 20, 20);
	
}

//--------------------------------------------------------------
void testApp::exit(){
	camera.exit();
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	camera.startCapture();
	camera.record();
	
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	player.play(60,127);
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

