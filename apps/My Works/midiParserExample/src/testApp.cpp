#include "testApp.h"
#include "ofxMidi.h"
//#include "midiparser.h"
//--------------------------------------------------------------
void testApp::setup(){
	string filename = ofToDataPath("VOC_SALAD_1.mid");
	
	//string filename = ofToDataPath("DRM_BOY_1.xml");
	
	//string filename = ofToDataPath("GTR_SALAD_SONG.mid");
	if (midi.loadMidi(filename)) {
		midi.dumpTracks();
	}
	
	/*
	filename = ofToDataPath("VOC_SALAD_SONG.mid");
	if (midi.loadMidi(filename)) {
		midi.dumpTracks();
	}
	
	filename = ofToDataPath("DRM_SALAD_SONG.mid");
	if (midi.loadMidi(filename)) {
		midi.dumpTracks();
	}
	 */
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

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

