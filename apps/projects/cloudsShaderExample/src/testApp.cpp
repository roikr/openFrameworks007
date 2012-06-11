#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(50, 50, 200);
	ofSetVerticalSync(false);
	
	
    clouds.setup();
}

//--------------------------------------------------------------
void testApp::update(){
    clouds.update();

}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(255);
	
	clouds.draw();
    
    ofSetColor(225);
	ofDrawBitmapString("'s' toggles shader", 10, 20);
    ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()),10,40);
	//ofSetColor(245, 58, 135);
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
		
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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

