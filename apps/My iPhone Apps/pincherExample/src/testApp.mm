#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofxRegisterMultitouch(this);
		
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	counter = 0;
	ofSetCircleResolution(50);
	ofBackground(255,255,255);
	bSmooth = false;
	
	
	
	pincher.setup(ofPoint(0,-140),0.7);
	
}

//--------------------------------------------------------------
void testApp::update(){
	counter = counter + 0.033f;
}

//--------------------------------------------------------------
void testApp::draw(){

	pincher.transform();
	
	
	
	
	//--------------------------- rectangles
	ofFill();
	for (int i = 0; i < 200; i++){
		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
		ofRect(ofRandom(250,350),ofRandom(350,450),ofRandom(10,20),ofRandom(10,20));
	}
	ofSetColor(0x000000);
	ofDrawBitmapString("rectangles", 275,500);


	pincher.draw();

}



void testApp::touchDown(int x, int y, int id) {
	
	pincher.touchDown(x, y, id);	
	
	bSmooth = true;
	

}


void testApp::touchMoved(int x, int y, int id) {
	
	pincher.touchMoved(x, y, id);
	
}

void testApp::touchUp(int x, int y, int id) {
	bSmooth = false;
	
		
	pincher.touchUp(x, y, id);
	
}

void testApp::touchDoubleTap(int x, int y, int id) {
}




