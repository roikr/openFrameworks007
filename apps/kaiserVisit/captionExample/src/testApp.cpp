#include "testApp.h"

#define SCREEN_RADIUS 150.0f
#define ANCHOR_RADIUS 50.0f
#define MARGIN 100.0f


//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(75);
    ofSetColor(200);
    
    
    
    
    caption.setup(ofRectangle(0,100, ofGetWidth(), ofGetHeight()-200), MARGIN, SCREEN_RADIUS);
    caption.setAnchor(ofVec2f(ofGetWidth()/2,ofGetHeight()/2));
   }

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofVec2f vec = ofVec2f(ofGetMouseX(),ofGetMouseY())-caption.getPos();
    vec = vec.normalized()*(vec.length()-ANCHOR_RADIUS);
    ofLine(caption.getPos(), caption.getPos()+vec);
    ofRectangle rect;
    rect.setFromCenter(caption.getPos(), 120, 90);
    ofRect(rect);
   
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    caption.setAnchor(ofVec2f(x,y));
     
   
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