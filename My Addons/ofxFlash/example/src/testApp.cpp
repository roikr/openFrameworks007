#include "testApp.h"
#include "ofxXmlSettings.h"


//--------------------------------------------------------------
void testApp::setup(){
        
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofBackground(255);
    
    doc.setup("DOMDocument.xml");
    
    doc.zoom = 1;
    doc.offset = -0.5*ofVec2f(ofGetWidth(),ofGetHeight())/doc.zoom;
    
    layoutID = doc.itemsMap["Start"];
    cout << "layoutID: " << layoutID << endl;
   
//    layout.font.setGlobalDpi(72);
//    layout.font.loadFont("LetterGothicStd.otf", 72,true,false);  // antialiased, no full charset
//    layout.lineHeight = 1.2;
//    
    doc.load();
    

}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255);

    ofPushMatrix();
//    float zoom = 1;
//    ofVec2f center = -0.5*ofVec2f(ofGetWidth(),ofGetHeight())/zoom;
//    ofScale(zoom, zoom);
    doc.symbolItems[layoutID].draw();

    ofPopMatrix();

    glColor4f(0,0,0,1);
	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );
}

//--------------------------------------------------------------
void testApp::exit(){
    doc.release();
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
    doc.symbolItems[layoutID].hitTest(ofVec2f(x,y));
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