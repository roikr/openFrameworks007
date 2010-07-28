#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	
	ofBackground(255,255,255);
	ofSetWindowTitle("arc length example");
	
	ofSetFrameRate(60);
	bSmooth = false;
	length = 0;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	//---------------------------  lines
	// a bunch of red lines, make them smooth if the flag is set
	
	if (curve.size()>1) {
		
		if (bSmooth){
			ofEnableSmoothing();
		}
		
		
		ofSetColor(0xFF0000);
		
		
				
		for (vector<ofPoint>::iterator iter = curve.begin(); iter!= curve.end()-1 ; iter++){
			ofPoint p1=*iter;
			ofPoint p2=*(iter+1);
			ofLine(p1.x,p1.y,p2.x, p2.y);
		}
		
		if (bSmooth){
			ofDisableSmoothing();
		}
	
		
	}
	
	ofSetColor(0x000000);
	ofDrawBitmapString("press 's' to toggle smoothness", 20,20);
	ofDrawBitmapString("length: "+ofToString(length, 0), 20, 40);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 's'){
		bSmooth = !bSmooth;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}



//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	curve.push_back(ofPoint(x,y));
	
	length = 0;
	lastPoint = ofPoint(x,y);
	
	
	detector.setup();
	detector.addPoint(lastPoint);
	
	currentCorner = 0;
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	curve.push_back(ofPoint(x,y));
	
	ofPoint p2=ofPoint(x,y);
	float sum = pow(p2.x-lastPoint.x, 2) + pow(p2.y-lastPoint.y, 2);
	if ( sum!=0) {
		length+=sqrt(sum);
	}
	lastPoint = p2;
	
	detector.addPoint(p2);
	detector.update();
	
	
	if (detector.getNumCorners() > currentCorner) {
		printf("currentCorner: %i\n", currentCorner );
		cout << "corner index: " << detector.getCornerIndex(currentCorner) << endl;
		cout << "path to corner: " << detector.getPathLength(0, detector.getCornerIndex(currentCorner)) << endl;
		cout << "current length" << length << endl;
		currentCorner = detector.getNumCorners();
	}
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	curve.clear();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

