#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    canvas.loadImage("canvas.png");
 
    
    ofBackground(255);
    ofEnableAlphaBlending();
    ofImage tip;
    tip.loadImage("brush_256.png");
    
    brush.setup(tip,10,ofGetWidth(), ofGetHeight());
    brush.setColor(ofColor(255,0,0));
    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255);
    canvas.draw(0,0);
    brush.draw();
    
    
    
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
   brush.touch(ofVec2f(x,y));
   
}
    

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    brush.clear();
    brush.touch(ofVec2f(x,y));
   

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
//    brush.touch(ofVec2f(x,y));
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