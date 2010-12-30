#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofxRegisterMultitouch(this);
		
	//ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);

	counter = 0;
	ofSetCircleResolution(50);
	ofBackground(255,255,255);
	bSmooth = false;
	
	
	image.loadImage("esb2.jpg");
	//pincher.setup(ofPoint(0,-140),0.7,pincherPrefs(480,320,ofRectangle(0,0,700,700),0.5,1.5));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(480,320,ofRectangle(0,0,1628,1085),0.1,15));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(480,320,ofRectangle(0,0,2912,4368),0.1,15));
	//pincher.setup(ofPoint(0,0),0.3,pincherPrefs(320,480,ofRectangle(0,0,728,1092),0.1,10));
	pincher.setup(ofPoint(0,0),0.3,pincherPrefs(320,480,ofRectangle(0,0,1365,2048),0.234,10));
	
	//pincher.setup(ofPoint(0,0),0.5,pincherPrefs(480,320,ofRectangle(0,0,700,700),0.5,1));
	
}

//--------------------------------------------------------------
void testApp::update(){
	image.update();
	counter = counter + 0.033f;
	if (pincher.getIsAnimating()) {
		pincher.update((float)(ofGetElapsedTimeMillis() - animStart)/250.0);
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	pincher.transform();
	
	image.draw(0,0);
	
	
	
	//--------------------------- rectangles
//	ofFill();
//	for (int i = 0; i < 200; i++){
//		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
//		ofRect(ofRandom(250,350),ofRandom(250,350),ofRandom(10,20),ofRandom(10,20));
//	}
//	ofSetColor(0x000000);
//	ofDrawBitmapString("rectangles", 275,500);
//
//
//	pincher.draw();

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
	if (!pincher.getIsAnimating()) {
		animStart = ofGetElapsedTimeMillis();
		pincher.touchDoubleTap(x,y,id);
	}
	
}




