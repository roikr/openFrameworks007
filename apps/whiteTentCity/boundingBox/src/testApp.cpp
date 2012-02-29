#include "testApp.h"
#include "ofxProCamToolkit.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
    
    font.loadFont("Guttman Hatzvi-Normal.ttf", 60,true,true);
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
//	glEnable(GL_DEPTH_TEST);
	
	// this sets the camera's distance from the object
	cam.setDistance(100);
    model.loadModel("model.dae");
	objectMesh = model.getMesh(0);
    
    cout<< "numIndices: " << objectMesh.getNumIndices() << endl;
    
    selection.assign(4, make_pair(0, false));
    choice = 0;
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground(0);
    
    bProject = false;
    
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
    
        ofMesh imageMesh = getProjectedMesh(objectMesh);
    
        ofSetColor(0, 0, 255,100);
        ofBeginShape();
            for (vector<ofVec3f>::iterator iter=q.screen.begin(); iter!=q.screen.end(); iter++) {
                ofVertex(*iter);
                
            }
        ofEndShape(true);
    
	cam.end();
    
   
    
//    ofFill();
//    ofSetColor(255);
//    float distance;
//    int choice = getClosestTripletOnMesh(objectMesh,imageMesh, mouseX, mouseY, &distance);
//    ofBeginShape();
//    for (int j=0;j<3;j++) {
//        ofVertex(imageMesh.getVerticesPointer()[objectMesh.getIndexPointer()[3*choice+j]]);
//    }
//     ofEndShape(true);
    
    
    
    ofSetColor(150);
    
    for (int i=0; i<selection.size(); i++) {
        if (selection[i].second) {
            ofVec3f &pos = imageMesh.getVerticesPointer()[selection[i].first];
            ofCircle(pos, 5);
            ofDrawBitmapString(ofToString(i+1), pos+ofVec2f(5, 25));
        }
        
        
    }
    
    if (bProject) {
        glPushMatrix();
        glMultMatrixf(q.glMatrix.getPtr());
        glPushMatrix();
        
        
        ofFill();
		ofSetColor(255);
        
        
        //ofCircle(ofGetWidth()/2, ofGetHeight()/2, ofGetHeight()/2);   
        wstring message = L"יתרבח קדצ שרוד םעה";
        font.drawString(message, 50, font.stringHeight(message));
        
        
        glPopMatrix();
        glPopMatrix(); 
    }
    
    
    
    ofSetColor(255);
    
    float distance;
    ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
    
   
    ofCircle(selected, 5);
    ofDrawBitmapString(ofToString(choice), selected+ofVec2f(5, -25));

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
            
        case '1':
        case '2':
        case '3':
        case '4': {
            int i = key-'1';
            if (selection[i].second) {
                selection[i].second = false;
            } else {
                selection[i].first = choice;
                selection[i].second = true;
            }
            
        } break;
            
        case 'b':
        case 'B': {
            if (q.screen.empty()) {
                
                vector<ofVec3f> input;
                for (int i=0; i<selection.size(); i++) {
                    if (selection[i].second) {
                        input.push_back(objectMesh.getVerticesPointer()[selection[i].first]);
                    }
                }
                vector<ofVec3f> bb = findBoundingBox(input);
                q.screen = findScreenCorners(bb, ofGetWidth(), ofGetHeight(), 10.0/500.0);
            } else {
                q.screen.clear(); 
            }
        } break;
            
        case 'p':
        case 'P': {
            if (bProject) {
                bProject = false;
            } else if (!q.screen.empty()) {
                bProject = true;
                
                vector<ofVec3f> src;
                src.push_back(ofVec3f(0,0));
                src.push_back(ofVec3f(ofGetWidth(),0));
                src.push_back(ofVec3f(ofGetWidth(),ofGetHeight()));
                src.push_back(ofVec3f(0,ofGetHeight()));
                
                cam.begin();
                                
                vector<ofVec3f> dst;
                for (vector<ofVec3f>::iterator iter = q.screen.begin(); iter!=q.screen.end(); iter++) {
                    ofVec3f cur = ofWorldToScreen(*iter);
                    dst.push_back(ofVec2f(cur.x,cur.y));
                }
                
                findHomography(src.data(), dst.data(), q.glMatrix.getPtr());
                q.matrix = ofMatrix4x4::getTransposedOf(q.glMatrix);
 
                
                
                
                cam.end();
                
            }
            
        } break;
            
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
