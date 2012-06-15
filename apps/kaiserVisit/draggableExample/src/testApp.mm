#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	bikers.loadImage("images/bikers.jpg");
    
    drag.setup(ofRectangle(200,200,bikers.getWidth(),bikers.getHeight()));
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetColor(0xFFFFFF);
	drag.begin();
	bikers.draw(0,0);
    drag.draw();
    drag.end();
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    drag.touchDown(touch);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    drag.touchMoved(touch);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    drag.touchUp(touch);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

