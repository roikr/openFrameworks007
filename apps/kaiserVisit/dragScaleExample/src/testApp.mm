#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
    ofEnableAlphaBlending();

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	image.loadImage("images/bikers.jpg");
        
    ofRectangle window(50,50,ofGetWidth()-100,ofGetHeight()-100);
    
       
    
//    drag.setup(ofRectangle(200,200,bikers.getWidth(),bikers.getHeight()),ofRectangle(0,0,ofGetWidth(),ofGetHeight()));
    
    drag.setup(window,image.getWidth(),image.getHeight());
    
    drag.setMaxZoom(10);
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
    drag.update();
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
    
    drag.draw();
    
    
    drag.begin();
    image.draw(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
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
    drag.animateScale(ofVec2f(touch.x,touch.y), 2);
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

