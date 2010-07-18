#include "testApp.h"

enum  {
	RECORD_MODE,
	EDIT_MODE
};

//--------------------------------------------------------------
void testApp::setup(){
	camWidth 		= 320;	// try to grab at this size. 
	camHeight 		= 240;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
	
	saver.listCodecs();
	//saver.setCodecType(2); ZACH FIX see recording quicktime with sound in sync on of forum
	saver.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_NORMAL);
	saver.setup(camWidth,camHeight,"output.mov");
	
	ofSetFrameRate(25);
	
	bRecording = false;
	bPlaying = false;
	bFrameByFrame = false;
	
	mode = RECORD_MODE;
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	
	switch (mode) {
		case RECORD_MODE:
			vidGrabber.grabFrame();
			
			
			if (bRecording == true){
				saver.addFrame(vidGrabber.getPixels(), 1.0f / 25.0f); 
				
				//  you can also pass in the frameRate:
				//	saver.addFrame(vidGrabber.getPixels(), 1.0f / 30.0f);  // 30 fps for this frame
				//	saver.addFrame(vidGrabber.getPixels(), 1.0f / 5.0f);   // 5 fps for this frame, etc....
				
			}
			break;
		case EDIT_MODE:
			vidPlayer.update();
			break;
		default:
			break;
	}
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(100,100,100);
	ofSetColor(0xffffff);
	
	
	switch (mode) {
		case RECORD_MODE:
			vidGrabber.draw(0,0);
			if (saver.bAmSetupForRecording()){
				ofSetColor(0xffffff);
				ofDrawBitmapString("setup for recording, (press 's' to save)",100,300);
				
				if (bRecording){
					ofSetColor(0x00ff00);
					ofDrawBitmapString("recording, (press 'r' to toggle)",100,320);
				} else {
					ofSetColor(0xff0000);
					ofDrawBitmapString("not recording, (press 'r' to toggle)",100,320);
				}
			} else {
				ofSetColor(0xffffff);
				ofDrawBitmapString("not setup for recording (press 'a' to start)",100,300);
				ofSetColor(0xaaaaaa);
				ofDrawBitmapString("not setup to record",100,320);
			}
			break;
		case EDIT_MODE:
			vidPlayer.draw(0, 0);
			ofSetColor(0x00ff00);
			ofDrawBitmapString("playing",100,300);
			break;
		default:
			break;
	}
	
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (mode) {
		case RECORD_MODE:
			switch (key) {
				case 'a':
					mode = RECORD_MODE;
					saver.setup(320,240,"output.mov");
					break;
				case 's':
					saver.finishMovie();
					bRecording = false;
					break;
				case 'r':
					bRecording = !bRecording;
					break;
					
				case 'p':
					mode = EDIT_MODE;
					vidPlayer.loadMovie("output.mov");
					vidPlayer.play();
					bPlaying = true;
				default:
					break;
			}
			break;
		case EDIT_MODE:
			switch (key) {
				
				case 'r':
					vidPlayer.stop();
					bPlaying = false;
					mode = RECORD_MODE;

					break;
					
				case 's':
					break;
					
				case 'e':
					break;
					
					
				default:
					break;
			}
			break;
		default:
			break;
	}
	
	
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

