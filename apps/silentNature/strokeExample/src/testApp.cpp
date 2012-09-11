#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofBackground(255);
    ofEnableAlphaBlending();
    ofBackground(127,127,127);
    ofSetColor(50);
    stroke.setup(5);
    
    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofNoFill();
    
    for (vector<ofVec2f>::iterator iter=stroke.getCurve().begin();iter!=stroke.getCurve().end();iter++) {
        ofRectangle rect;
        rect.setFromCenter(*iter, 1, 1);
        ofRect(rect);
    }
    
    
    
    for (vector<ofVec2f>::iterator iter=drags.begin();iter!=drags.end();iter++) {
        ofRectangle rect;
        rect.setFromCenter(*iter, 3, 3);
        ofRect(rect);
    }
    
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
//    cout << "mouseDragged" << endl;
   stroke.touch(ofVec2f(x,y));
    drags.push_back(ofVec2f(x,y));
   
}
    

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    stroke.clear();
    drags.clear();
    stroke.touch(ofVec2f(x,y));
   

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