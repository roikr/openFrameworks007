#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
	
    scroll.setup(ofRectangle(0,0,200, ofGetHeight()), 10, 10);
    scroll.addItem("images/bike_icon.gif");
    scroll.addItem("images/bike_icon.png");
    scroll.addItem("images/bikers.jpg");
    scroll.addItem("images/gears.gif");
    scroll.addItem("images/tdf_1972_poster.jpg");
    scroll.addItem("images/transparency.png");
    scroll.addItem("images/bike_icon.gif");
    scroll.addItem("images/bike_icon.png");
    scroll.addItem("images/bikers.jpg");
    scroll.addItem("images/gears.gif");
    scroll.addItem("images/tdf_1972_poster.jpg");
    scroll.addItem("images/transparency.png");
	
    
    
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
    scroll.update();
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
	scroll.draw();
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    scroll.touchDown(touch);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    scroll.touchMoved(touch);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    scroll.touchUp(touch);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    scroll.touchDoubleTap(touch);
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

