#include "testApp.h"






//--------------------------------------------------------------
void testApp::setup(){
    origami.setup();
    ofBackground(100);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    origami.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'd':
            origami.dump();
            break;
            
        case 'c':
            origami.fold(origami.cuts.size()-1, true);
            break;
        case 'a':
            origami.fold(origami.cuts.size()-1, false);
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
    downPos = ofVec2f(x,y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    origami.split(downPos, ofVec2f(x,y));
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