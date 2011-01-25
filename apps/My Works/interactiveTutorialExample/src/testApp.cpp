#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	tutorial.setup();	
	tutorial.loadFile("tutorial.xml");
	counter = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	tutorial.update();
	if (tutorial.getState()==TUTORIAL_IDLE) {
		tutorial.start();
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(0, 0, 0);
	
	if (tutorial.getState() == TUTORIAL_READY) {
		ofDrawBitmapString(tutorial.getCurrentText(), 20, 20);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	tutorial.done(key-'0');
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
	
	
	switch (tutorial.getState()) {
		case TUTORIAL_IDLE:
			tutorial.start();
			break;
		
		case TUTORIAL_READY:
			tutorial.done(counter);
			counter++;
			break;

	}
	
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

