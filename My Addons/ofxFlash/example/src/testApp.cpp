#include "testApp.h"
#include "ofxXmlSettings.h"


//--------------------------------------------------------------
void testApp::setup(){
        
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofBackground(255);
    
    doc.setup("DOMDocument.xml");
    
    ofMatrix4x4 mat;
    mat.translate(100, 50, 0);
    mat.scale(0.5, 0.5, 1.0);
    layout = doc.getSymbolItem("Start")->createInstance("layout",mat);

    
   
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

    
    layout.draw();

   

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
   
    vector<ofxSymbolInstance> items = layout.hitTest(ofVec2f(x,y));
    
    for (vector<ofxSymbolInstance>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if (iter->type == SYMBOL_INSTANCE) {
            cout << iter->name << "\t";
        }
    }
    
    cout << endl;
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