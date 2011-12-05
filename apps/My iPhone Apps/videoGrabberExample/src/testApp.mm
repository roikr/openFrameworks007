#include "testApp.h"

#include "ofxiPhoneVideo.h"
#import "EAGLView.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofxRegisterMultitouch(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
//	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	printf("width: %i, height %i\n",ofGetWidth(),ofGetHeight());
	
	vagRounded.loadFont("vag.ttf", 12);
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
	
	grabber.setup(&video,FRONT_CAMERA);
	
	
	
	player.setup(&video,true);
	bRecording = false;
	
	bCameraOffset = false;
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	
	if  (grabber.getState() == CAMERA_READY) {
        grabber.startCamera();
    }
        
    grabber.update();
    
    if (!bCameraOffset  && grabber.getState() >= CAMERA_RUNNING) {
        bCameraOffset = true;
        ofPoint offset;
        float scale = 0.75;
        offset = ofPoint((grabber.getCameraWidth()-video.textureWidth/scale)/2,0);
		
		
		grabber.setTransform(offset,scale);
        grabber.startCapture();
		player.stop();
        
		
	}
	
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
	
	grabber.render(); //ofPoint((grabber.getCameraWidth()-video.textureWidth)/2,(grabber.getCameraHeight()-video.textureHeight)/2));
	[ofxiPhoneGetGLView() startRender];
	
	
	glViewport(0, 0, ofGetWidth(), ofGetHeight());
	
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D (0, ofGetWidth(), 0, ofGetHeight());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
	glScalef(1.0, -1.0,1.0);
	glTranslatef(0, -ofGetHeight(), 0);
//	
	ofBackground(0,0,0);	
	ofSetHexColor(0xffffff);
	
	ofPushMatrix();
	
	ofTranslate(ofGetWidth(), 0, 0);
	ofRotate(90);
	grabber.drawCamera();
	
	if (grabber.getState()>=CAMERA_CAPTURING) {
		grabber.draw();
	}
	else if (!video.textures.empty()) {
		player.draw();
	}
	ofPopMatrix();
	

//	ofTranslate(-60, -60, 0);

	sprintf (timeString, "time: %i:%i:%i \nelapsed time %i", ofGetHours(), ofGetMinutes(), ofGetSeconds(), ofGetElapsedTimeMillis());
	
	ofSetHexColor(0xffffff);
	vagRounded.drawString(eventString, 20,198);
	
	ofSetColor(255,122,220);
	vagRounded.drawString(eventString, 22,200);
	
	ofSetHexColor(0xffffff);
	vagRounded.drawString(timeString, 20,98);
	
	ofSetColor(255,122,220);
	vagRounded.drawString(timeString, 22,100);	
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
	
//	grabber.cameraToggle();
	
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

