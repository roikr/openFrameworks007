#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    fbo.allocate(500, 500);
}

//--------------------------------------------------------------
void testApp::update(){
    ofSetOrientation(OF_ORIENTATION_DEFAULT);
    fbo.begin();
    ofClear(0);
    ofRect(0, 0, 100, 50);
    fbo.end();
    ofSetOrientation(OF_ORIENTATION_90_LEFT);
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    fbo.draw(0, 0);
    ofRect(0, 0, 100, 50);
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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}