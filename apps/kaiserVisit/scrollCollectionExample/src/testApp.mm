#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    
    
	
    hScroll.setup(scrollCollectionPrefs(true, 100,ofGetHeight(),10,10,2,0xFF0000,100));
    
    
    ofDirectory dir;
    dir.listDir(ofToDataPath("images"));
    
    for (int i=0;i<dir.numFiles();i++) {
        ofImage img;
        cout << dir.getName(i) << endl;
        img.loadImage(dir.getFile(i));
        hScroll.addItem(img, dir.getName(i));
        hScroll.addItem(img, dir.getName(i));
    }
    
    
   
    
//    vScroll.setup(scrollCollectionPrefs(true,ofRectangle(ofGetWidth()-100,0,100, ofGetHeight()),20,20,5,0x00FF00,100));
//    
//    vScroll.addItem("images/bike_icon.gif");
//    vScroll.addItem("images/bike_icon.png");
//    vScroll.addItem("images/bikers.jpg");
//    vScroll.addItem("images/gears.gif");
//    vScroll.addItem("images/tdf_1972_poster.jpg");
//    vScroll.addItem("images/transparency.png");
//    vScroll.addItem("images/bike_icon.gif");
//    vScroll.addItem("images/bike_icon.png");
//    vScroll.addItem("images/bikers.jpg");
//    vScroll.addItem("images/gears.gif");
//    vScroll.addItem("images/tdf_1972_poster.jpg");
//    vScroll.addItem("images/transparency.png");
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
    hScroll.update();
    vScroll.update();
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
	hScroll.draw();
	vScroll.draw();
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    hScroll.touchDown(touch);
    vScroll.touchDown(touch);
    
    if (hScroll.getIsInside(ofVec2f(touch.x,touch.y))) {
        cout << "touch inside" << endl;
        
    }
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    hScroll.touchMoved(touch);
    vScroll.touchMoved(touch);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    hScroll.touchUp(touch);
    vScroll.touchUp(touch);
    if (hScroll.getIsSelected()) {
        cout << "selected: " << hScroll.getSelectedID() << endl;
    }
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    hScroll.touchDoubleTap(touch);
    vScroll.touchDoubleTap(touch);
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

