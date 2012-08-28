#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    nat.setup();
   
}

//--------------------------------------------------------------
void testApp::update(){
    nat.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    nat.draw();
    
    glColor4f(1,1,1,1);
	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    emulator.keyPressed(key);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
//    cout << x << "\t" << y << "\t" << button << endl;
    if (emulator.getShouldMove()) {
        ofTouchEventArgs touch = emulator.mouseDragged(x, y);
        nat.touchMoved(touch);
    }
    

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    
    
    ofTouchEventArgs touch = emulator.mousePressed(x, y, button);
    nat.touchDown(touch);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
    
    ofTouchEventArgs touch = emulator.mouseReleased(x, y, button);
    nat.touchUp(touch);
    
        
    
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