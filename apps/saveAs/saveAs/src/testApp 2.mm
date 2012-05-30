#include "testApp.h"
#include "ofxXmlSettings.h"
//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    ofxXmlSettings xml;
    
    if (xml.loadFile("layout/DOMDocument.xml")) {
        xml.pushTag("DOMDocument");
        xml.pushTag("timelines");
        xml.pushTag("DOMTimeline");
        xml.pushTag("layers");
        for (int j=0;j<xml.getNumTags("DOMLayer");j++) {
            if (xml.getAttribute("DOMLayer", "name", "",j) == "Layer 1") {
                xml.pushTag("DOMLayer",j);
                xml.pushTag("frames");
                xml.pushTag("DOMFrame");
                xml.pushTag("elements");
                for (int i=0; i<xml.getNumTags("DOMBitmapInstance"); i++) {
                    bitmapInstance bi;
                    xml.pushTag("DOMBitmapInstance",i);
                    xml.pushTag("matrix");
                    bi.pos = ofVec2f(xml.getAttribute("Matrix", "tx", 0.0),xml.getAttribute("Matrix", "ty", 0.0));
                    bi.scale = xml.getAttribute("Matrix", "a", 0.0);
                    xml.popTag();
                    xml.popTag();
                    elements.push_back(bi);
                    elements.back().image.loadImage("layout/LIBRARY/"+xml.getAttribute("DOMBitmapInstance", "libraryItemName", "",i));
                    elements.back().image.update();
                }
                xml.popTag();
                xml.popTag();
                xml.popTag();
                xml.popTag(); 
                break;
            }
        }
        
        xml.popTag();
        xml.popTag();
        xml.popTag();
        xml.popTag();
    }
    
    //    pincher.setup(ofPoint(0,0), FULL_SCREEN_SCALE,pincherPrefs(ofGetWidth(),ofGetHeight(),ofRectangle(0,0,2048,1024),FULL_SCREEN_SCALE,1.8));
	
    pincher.setup(ofPoint(0,0), 1,pincherPrefs(ofGetWidth(),ofGetHeight(),ofRectangle(0,0,2048,1024),0.5,1.8));
	
    
    ofEnableAlphaBlending();
    ofBackground(75);
	
}

//--------------------------------------------------------------
void testApp::update(){
    pincher.update(1);
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofPushMatrix();
    
    pincher.transform();
    
	for (vector<bitmapInstance>::iterator iter=elements.begin(); iter!=elements.end(); iter++) {
        ofPushMatrix();
        ofTranslate(iter->pos.x, iter->pos.y);
        ofScale(iter->scale, iter->scale);
        iter->image.draw(0, 0);
        ofPopMatrix();
    }
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    pincher.touchDown(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    pincher.touchMoved(touch.x, touch.y, touch.id);

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    pincher.touchUp(touch.x, touch.y, touch.id);

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    pincher.touchDoubleTap(touch.x, touch.y, touch.id);

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

