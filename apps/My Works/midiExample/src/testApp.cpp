#include "testApp.h"
#include "ofxMidi.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofxMidi midi;
	
	midi.loadFromXml("Muppet_Show.xml");
	
	event e;
	while (midi.getCurrentEvent(e)) {
		cout << "absolute: " << e.absolute << ", channel: " << e.channel << ", note: " << e.note << ", velocity: " << e.velocity << endl;
		midi.nextEvent();
	}
	
}

//--------------------------------------------------------------
void testApp::update(){
	
		
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(100,100,100);
		
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
		
	
	
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

