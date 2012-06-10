#include "testApp.h"

#define SAMPLE_XML_PATH "SamplesConfig.xml"




//--------------------------------------------------------------
void testApp::setup(){
       
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    
    cam.setup();
    
    depthTexture.allocate(cam.getDepthPixels().getWidth(), cam.getDepthPixels().getHeight(), GL_LUMINANCE);
    colorTexture.allocate(cam.getImagePixels().getWidth(),cam.getImagePixels().getHeight(),GL_RGB);
   
    
    
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    cam.update();
    
    
    depthTexture.loadData(cam.getDepthPixels().getPixels(),cam.getDepthPixels().getWidth(), cam.getDepthPixels().getHeight(), GL_LUMINANCE);
    
    colorTexture.loadData(cam.getImagePixels().getPixels(), cam.getImagePixels().getWidth(), cam.getImagePixels().getHeight(), GL_RGB);
    
       
   
}

//--------------------------------------------------------------
void testApp::draw(){
//
    colorTexture.draw(0, 0);
    depthTexture.draw(colorTexture.getWidth(),0);
    
    
}

void testApp::exit() {
    depthTexture.clear();
    colorTexture.clear();
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