#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
    
    image.setup("1280_800_test.png",2048);
    image.load();
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
//    vector<pair<int, int> > modes = extView.listModes();
//    
//	
//    ss << "External Screen: " << endl;
//    
//    
//    if (!modes.empty()) {
//        vector<pair<int, int> >::iterator iter;
//        vector<pair<int, int> >::iterator maxIter = modes.begin();
//        
//        for (iter=modes.begin(); iter!=modes.end(); iter++) {
//            ss << "Screen mode: " << iter->first << "x" << iter->second   << endl;
//            
//            if (iter->first == 1280 && iter->second == 800) {
//                break;
//            }
//            
//            if (iter->first * iter->second > maxIter->first * maxIter->second ) {
//                maxIter = iter;
//            }
//        }
//        
//        if (iter==modes.end()) {
//            iter = maxIter;
//        }
//        
//        extView.setupMode(distance(modes.begin(), iter));
//        ss << "Selected mode: " << iter->first << "x" << iter->second   << endl;
//    }
    

        
	ofBackground(127,127,127);
    ofEnableAlphaBlending();
    selected = 0;
    ofSetLineWidth(3);
    
}

//--------------------------------------------------------------
void testApp::update(){
    if (extView.getIsActive()) {
        extView.begin();
        image.draw();
        extView.end();
    }

}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255);
    image.draw();
    
    
    vector<pair<int, int> > modes = extView.listModes();
   
    if (!modes.empty()) {
        float size = (float)ofGetWidth()/(float)modes.size();
        for (vector<pair<int, int> >::iterator iter=modes.begin(); iter!=modes.end(); iter++) {
            int dist = distance(modes.begin(), iter);
            ofFill();
            if (dist==selected) {
               ofSetColor(0, 255, 0,  100);
            } else {
                ofSetColor(0, 0, 255, 100);
            }
            
            
            
            
           
            ofRect( dist*size,ofGetHeight()-50, size,50);
            ofNoFill();
            ofSetColor(255);
            ofRect( dist*size,ofGetHeight()-50, size,50);
            stringstream ss;
            ss << iter->first << "x" << iter->second   << endl;
            ofDrawBitmapString(ss.str(), dist*size+20,ofGetHeight()-25);
            
            
        }
        
    }

    
	
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    vector<pair<int, int> > modes = extView.listModes();   
    if (!modes.empty() && touch.y>ofGetHeight()-50) { 
        selected = floor((float)touch.x/(float)(ofGetWidth()+1)*modes.size());
        extView.setupMode(selected);
    }
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

