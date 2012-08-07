#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	// IMPORTANT!!! if your sound doesn't work in the simulator - read this post - which requires you set the output stream to 24bit 
	//	http://www.cocos2d-iphone.org/forum/topic/4159

	// register touch events
	ofRegisterTouchEvents(this);
	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	ofBackground(0);

	int bufferSize = 256;
	int sampleRate = 44100;
	
	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 4);	
    
	
	
	ofSetFrameRate(60);
    
    tex = ofRectangle(0,0,1,1);

}

//--------------------------------------------------------------
void testApp::update(){
    
    player.update();
    
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
   
    float aspectRatio = player.getWidth()/player.getHeight();
     float width = ofGetHeight()*aspectRatio;
    ofRectangle rect((ofGetWidth() - width)/2,0,width,ofGetHeight());
    
    player.draw(rect, tex);
	
    glColor4f(1,1,1,1);
	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );	
}

//--------------------------------------------------------------
void testApp::audioOut( float * output, int bufferSize, int nChannels ){
			
	player.audioRequested(output, bufferSize, nChannels);
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	player.stop();
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    player.play(ofToDataPath("big_test.mov"));
}


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs &touch){
	
}
