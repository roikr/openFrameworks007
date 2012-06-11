#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground( 40, 100, 40 );

	// open an outgoing connection to HOST:PORT
	sender.setup( HOST, PORT );
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	// display instructions
	string buf;
	buf = "sending osc messages to" + string( HOST ) + ofToString( PORT );
	ofDrawBitmapString( buf, 10, 20 );
	ofDrawBitmapString( "move the mouse to send osc message [/mouse/position <x> <y>]", 10, 50 );
	ofDrawBitmapString( "click to send osc message [/mouse/button <button> <\"up\"|\"down\">]", 10, 65 );
	ofDrawBitmapString( "press A to send osc message [/test 1 3.5 hello <time>]", 10, 80 );
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

