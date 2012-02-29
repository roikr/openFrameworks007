#include "testApp.h"
#include "ofxProCamToolkit.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	glEnable(GL_DEPTH_TEST);
	
	// this sets the camera's distance from the object
	cam.setDistance(200);
    model.loadModel("model.dae");
	objectMesh = model.getMesh(0);
    
    cout<< "numIndices: " << objectMesh.getNumIndices() << endl;
    
    ofBackground(0);
}

//--------------------------------------------------------------
void testApp::update(){
    
}




//--------------------------------------------------------------
void testApp::draw(){

    ofSetColor(100);
    
	cam.begin();
        //model.drawWireframe();
        objectMesh.drawWireframe();
    
        // check to see if anything is selected
        // draw hover point magenta
    objectMesh.drawWireframe();
        ofMesh imageMesh = getProjectedMesh(objectMesh);
    
	cam.end();
    
   
    
    ofFill();
    ofSetColor(255);
    float distance;
    int choice = getClosestTripletOnMesh(objectMesh,imageMesh, mouseX, mouseY, &distance);
    ofBeginShape();
    for (int j=0;j<3;j++) {
        ofVertex(imageMesh.getVerticesPointer()[objectMesh.getIndexPointer()[3*choice+j]]);
    }
     ofEndShape(true);
    
//    int choice;
//    float distance;
//    ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
//    
//    ofVec2f tooltipOffset(5, -25);
//    ofCircle(selected, 5);
//    ofDrawBitmapString(ofToString(choice), selected+tooltipOffset);

	ofSetColor(255);
	string msg = string("Using mouse inputs to navigate ('m' to toggle): ") + (cam.getMouseInputEnabled() ? "YES" : "NO");
	msg += "\nfps: " + ofToString(ofGetFrameRate(), 2);
	ofDrawBitmapString(msg, 10, 20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
		case 'M':
		case 'm':
			if(cam.getMouseInputEnabled()) cam.disableMouseInput();
			else cam.enableMouseInput();
			break;
			
		case 'F':
		case 'f':
			ofToggleFullscreen();
			break;
	}
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
