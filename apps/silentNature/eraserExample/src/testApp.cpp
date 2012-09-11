#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    canvas.loadImage("canvas.png");
 
    
    ofBackground(255);
    ofEnableAlphaBlending();
    
    fbo.allocate(ofGetWidth(),ofGetHeight());
    
    fbo.begin();
    glClearColor(0,0,0, 0);
    glClear( GL_COLOR_BUFFER_BIT);
    ofSetColor(255, 0, 0);
    ofCircle(0.5*ofVec2f(ofGetWidth(),ofGetHeight()), ofGetWidth()/2);
    fbo.end();
    
    
    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255);
    canvas.draw(0,0);
    fbo.draw(0, 0);
    
    
    
    
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
   eraser.touch(ofVec2f(x,y),fbo);
   
}
    

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    eraser.clear(15);
    eraser.touch(ofVec2f(x,y),fbo);
   

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    eraser.touch(ofVec2f(x,y),fbo);
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