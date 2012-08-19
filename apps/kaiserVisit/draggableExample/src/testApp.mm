#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
	
	bikers.loadImage("images/bikers.jpg");
    
    drag.setup(bikers.getWidth(),bikers.getHeight(),ofMatrix4x4());
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetColor(0xFFFFFF);
	drag.begin();
    ofTranslate(-0.5*ofVec2f(bikers.getWidth(),bikers.getHeight()));
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

