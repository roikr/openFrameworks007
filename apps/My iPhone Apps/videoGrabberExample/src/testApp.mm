#include "testApp.h"

#include "ofxiPhoneVideo.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	vagRounded.loadFont("vag.ttf", 18);
	ofBackground(50,50,50);	
	
	ofSetFrameRate(60);
	
	
	
	video.fps = 25;
	video.numIntroFrames = 4;
	
	video.numFrames = 25;
	video.sampleLength = 1000*video.numFrames/video.fps;
	

	video.widthFraction = 1.0;
	video.heightFraction = 1.0;
	video.textureWidth = 128;
	video.textureHeight = 256;
	video.bHorizontal = true;
	video.bFlipHoriznotal = false;
	
	grabber.setup(&video);
	grabber.startCamera();
	
	
	player.setup(&video,true);
	bRecording = false;
	
}

//--------------------------------------------------------------
void testApp::update(){
	counter = counter + 0.033f;
	player.update();
	
	switch (grabber.getState()) {
		
		case CAMERA_RECORDING:
			if (!bRecording) {
				bRecording = true;
			}
			break;
		case CAMERA_RUNNING:
			if (bRecording) {
				bRecording = false;
				player.playIntro();
			}
			break;
		default:
			
			break;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	grabber.render();
	
	ofBackground(0,0,0);	
	ofSetColor(0xffffff);
	
	
	if (grabber.getState() == CAMERA_CAPTURING || grabber.getState()==CAMERA_RECORDING) {
		grabber.draw();
	} else {
		player.draw();
	}

	

	ofTranslate(-60, -60, 0);

	sprintf (timeString, "time: %0.2i:%0.2i:%0.2i \nelapsed time %i", ofGetHours(), ofGetMinutes(), ofGetSeconds(), ofGetElapsedTimeMillis());
	
	ofSetColor(0xffffff);
	vagRounded.drawString(eventString, 98,198);
	
	ofSetColor(255,122,220);
	vagRounded.drawString(eventString, 100,200);
	
	ofSetColor(0xffffff);
	vagRounded.drawString(timeString, 98,98);
	
	ofSetColor(255,122,220);
	vagRounded.drawString(timeString, 100,100);	
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	sprintf(eventString, "touchDown = (%2.0f, %2.0f - id %i)", touch.x, touch.y, touch.id);
	switch (grabber.getState()) {
		case CAMERA_CAPTURING:
		case CAMERA_RECORDING:
			break;
		default:
			player.play(1.0f);
			break;
	}
	

}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	sprintf(eventString, "touchMoved = (%2.0f, %2.0f - id %i)", touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	sprintf(eventString, "touchUp = (%2.0f, %2.0f - id %i)",touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	sprintf(eventString, "doubleTap at (%2.0f, %2.0f)",touch.x, touch.y);
	switch (grabber.getState()) {
		case CAMERA_CAPTURING:
			grabber.record();
			break;
		case CAMERA_RECORDING:
			break;
		default:
			grabber.startCapture();
			break;
	}
	
}

//--------------------------------------------------------------
void testApp::lostFocus(){
	sprintf(eventString, "alert - lost focus");
}

//--------------------------------------------------------------
void testApp::gotFocus(){
	sprintf(eventString, "alert - got focus");
}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){
	sprintf(eventString, "alert - memory warning");
}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){
	sprintf(eventString, "alert - orientation change to %i", newOrientation);
}

