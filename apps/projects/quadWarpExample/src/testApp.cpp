#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){
    quadWarp.setup();
    ofBackground(0);
    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    quadWarp.draw();
    quadWarp.begin();
    ofNoFill();
    ofSetColor(255);
    ofRect(50, 50, ofGetWidth()-100, ofGetHeight()-100);
    quadWarp.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 't':
            ofToggleFullscreen();
            
            break;
            
        default:
            break;
    }
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



