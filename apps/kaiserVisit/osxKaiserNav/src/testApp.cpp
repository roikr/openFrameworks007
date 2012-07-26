#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    nav.setup();
    bMove = false;
}

//--------------------------------------------------------------
void testApp::update(){
    nav.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    nav.draw();
    
    glColor4f(1,1,1,1);
	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    bMove = false;
    if (find(buttons.begin(),buttons.end(),key-'1')!=buttons.end()) {
        bMove = true;
        current = key-'1';
    }
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
    if (bMove) {
        ofTouchEventArgs touch;
        touch.x = x;
        touch.y = y;
        touch.id = current;
        touch.numTouches = buttons.size();
        nav.touchMoved(touch);
    }

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    
    
    buttons.push_back(button);
    if (!bMove) {
        current = button;
        bMove = true;
    }
   
        
    ofTouchEventArgs touch;
    touch.x = x;
    touch.y = y;
    touch.id = button;
    touch.numTouches = buttons.size();
    nav.touchDown(touch);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
    
    ofTouchEventArgs touch;
    touch.x = x;
    touch.y = y;
    touch.id = button;
    touch.numTouches = buttons.size();
    nav.touchUp(touch);
    
    if (bMove && button == current) {
        bMove = false;
    }
    
    
    vector<int>::iterator iter=find(buttons.begin(),buttons.end(),button);
    if (iter!=buttons.end()) {
        buttons.erase(iter);
    }
        
        
    
    
    
    
    
    
    
    
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