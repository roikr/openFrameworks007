#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
    ofEnableAlphaBlending();

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
	
	image.loadImage("images/bikers.jpg");
        
    ofRectangle window(50,50,1024-100,768-100);
    
    ofMatrix4x4 mat;
    float scale = (float)ofGetHeight()/768;
    mat.scale(scale, scale, 1.0);
    mat.translate(0.5*(ofGetWidth()-scale*1024), 0, 0);
    
       
    
//    drag.setup(ofRectangle(200,200,bikers.getWidth(),bikers.getHeight()),ofRectangle(0,0,ofGetWidth(),ofGetHeight()));
    
    drag.setup(window,image.getWidth()+200,image.getHeight(),ofMatrix4x4::newScaleMatrix(2, 2, 1.0),mat);
    
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
    
    
    
    drag.begin();
    image.draw(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
    drag.end();
	
    drag.debugDraw();
    drag.draw();
    
	
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
    drag.animateScale(ofVec2f(touch.x,touch.y), 5);
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

