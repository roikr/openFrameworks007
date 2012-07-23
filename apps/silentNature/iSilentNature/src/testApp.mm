#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
//	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
	ofBackground(127,127,127);
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
    ofMatrix4x4 mat;
    layout = doc.getSymbolItem("Layout")->createInstance("layout",mat);
    layout.getChild("brush_open")->bVisible = false;
    layout.getChild("crayons_open")->bVisible = false;
    layout.getChild("colors")->bVisible = false;
    layout.getChild("patterns")->bVisible = false;
    
    
//    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
//        layout.mat.translate(64, 0, 0);
//        layout.mat.scale(5.0/6.0, 5.0/6.0, 1.0);
//    }
        
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;

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
    

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    vector<ofxSymbolInstance> items = layout.hitTest(ofVec2f(touch.x,touch.y));
    
    for (vector<ofxSymbolInstance>::iterator iter=items.begin(); iter!=items.end(); iter++) {
       
        if (iter->type == SYMBOL_INSTANCE) {
            cout << iter->name << "\t";
        }
    }
    
    cout << endl;
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

