#include "testApp.h"
#include "ofxProCamToolkit.h"
#include "ofxRKProCamToolkit.h"
#include "ofxXmlSettings.h"


enum {
    STATE_TENT,
    STATE_SCREEN,
    STATE_CITY
};

//--------------------------------------------------------------
void testApp::setup(){
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
    
    font.loadFont("Guttman Hatzvi-Normal.ttf", 50,true,true);
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
//	glEnable(GL_DEPTH_TEST);
	
	// this sets the camera's distance from the object
	//cam.setDistance(100);
   // cityModel.loadModel("TENTS_PIVOTS.obj",true);
    cityModel.loadModel("base6.obj",true);
    cityMesh = cityModel.getMesh(0);
    tentModel.loadModel("tent12.obj",true);
	tentMesh = tentModel.getMesh(0);
    
    cout<< "numIndices: " << cityMesh.getNumIndices() << endl;
    
    
    choice = 0;
    state = STATE_TENT;
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground(0);
    
    selection.assign(4, make_pair(0, false));
    
    bProject = false;
    bScreen = false;
    origin = 0;
    bTent = false;
    bOrigin = false;
    
    scale = PIXEL_PER_CENTIMETER;
    currentScreen = 0;
    
    overlay.load();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	glEnable(GL_DEPTH_TEST);
    
    //some model / light stuff
    glShadeModel(GL_SMOOTH);
    light.enable();
    
    //ofEnableSeparateSpecularLight();
    //    ofSetGlobalAmbientColor(ofColor(30,30,30));
    
    glEnable(GL_NORMALIZE);

}

//--------------------------------------------------------------
void testApp::update(){
    
}




void testApp::drawTent() {
    
    vector<wstring> words;
    words.push_back(L"םעה");
    words.push_back(L"שרוד");
    words.push_back(L"קדצ");
    words.push_back(L"יתרבח");
    
    for (vector<screen>::iterator siter=overlay.screens.begin(); siter!=overlay.screens.end(); siter++) {

        ofPushMatrix();
        glMultMatrixf(siter->glMat.getPtr());
        
        ofFill();
        
        if (bScreen) {
            ofPushStyle();
            ofSetColor(255, 0, 0);
            ofLine(0, 0, 200, 0);
            ofSetColor(0, 255, 0);
            ofLine(0, 0, 0, 200);
            ofSetColor(0, 0, 255,100);
            ofRect(0, 0, ofGetWidth(), ofGetHeight());
            ofPopStyle();
        }
        
                
        int dist = distance(overlay.screens.begin(),siter);
        
        if (dist<words.size()) {
            wstring word = words[dist];
            font.drawString(word, (WORD_LENGTH * PIXEL_PER_CENTIMETER - font.stringWidth(word))/2, font.stringHeight(word)); 
        }
        
        ofPopMatrix();
    }
}

void testApp::drawCity() {
    
    for (vector<tent>::iterator iter=overlay.tents.begin(); iter!=overlay.tents.end(); iter++) {
        ofPushMatrix();
        glMultMatrixf(iter->glMat.getPtr());
        drawTent();
        ofPopMatrix();
    }
}



void testApp::drawScreen() {
    
    if (!overlay.screens.empty()) {
        screen s = overlay.screens[currentScreen];
        
        ofVec3f p0 = tentMesh.getVerticesPointer()[s.indices[0]];
        ofVec3f p1 = tentMesh.getVerticesPointer()[s.indices[3]];
        
        float proj = s.xVec.dot(p1-p0);
        ofVec3f topLeft = p0+s.xVec*proj;
        
        ofSetColor(0, 0, 255,100);
        
        ofFill();
        
        ofBeginShape();
        for (vector<int>::iterator viter=s.indices.begin(); viter!=s.indices.end(); viter++) {
            
            ofVec3f vec = tentMesh.getVerticesPointer()[*viter]-topLeft;
            ofVec3f pos;
            pos.z = 0;
            pos.x = s.xVec.dot(vec)*scale;
            pos.y = s.yVec.dot(vec)*scale;
            ofVertex(pos);
        }
        ofEndShape(true);
        
        ofNoFill();
        ofSetColor(0, 255, 0,255);
        
        ofCircle(s.xVec.dot(s.origin-topLeft)*scale, s.yVec.dot(s.origin-topLeft)*scale, 5);
    }
    
}



//--------------------------------------------------------------
void testApp::draw(){
    
    switch (state) {
        case STATE_TENT: {
            
            ofSetColor(100);
            
            cam.begin();
            glEnable(GL_DEPTH_TEST);
            tentMesh.drawFaces();
            
            // check to see if anything is selected
            // draw hover point magenta
            
            ofMesh imageMesh = getProjectedMesh(tentMesh);
            
            if (bProject) {
                glDisable(GL_DEPTH_TEST);
                ofSetColor(255);
                drawTent();
                
            }

                        
            cam.end();            
            
            ofSetColor(150);
            
            for (int i=0; i<selection.size(); i++) {
                if (selection[i].second) {
                    ofVec3f &pos = imageMesh.getVerticesPointer()[selection[i].first];
                    ofCircle(pos, 5);
                    ofDrawBitmapString(ofToString(i+1), pos+ofVec2f(5, 25));
                }
            }
            
            ofSetColor(255);
            
            float distance;
            ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
            
            ofCircle(selected, 5);
            ofDrawBitmapString(ofToString(choice), selected+ofVec2f(5, -25));
            
            
        } break;
        case STATE_CITY: {
            
            
            cam.begin();
             glEnable(GL_DEPTH_TEST);
            ofSetColor(255);
            cityMesh.drawFaces();
            ofSetColor(100);
            for (vector<tent>::iterator iter=overlay.tents.begin(); iter!=overlay.tents.end(); iter++) {
                ofPushMatrix();
                glMultMatrixf(iter->glMat.getPtr());
                
                tentMesh.drawFaces();
                
                ofPopMatrix();
            }
            
            ofSetColor(255);
            if (bProject) {
                glDisable(GL_DEPTH_TEST);
                ofDisableBlendMode();
                ofDisableAlphaBlending();
                drawCity();
            }
            
            // check to see if anything is selected
            // draw hover point magenta
            
            ofMesh imageMesh = getProjectedMesh(cityMesh);
            
            
            if (bTent && !bOrigin) {
                
                ofVec3f p0 = cityMesh.getVerticesPointer()[choice];
                ofPushMatrix();
                glTranslatef(p0.x,p0.y,p0.z);
                tentModel.getMesh(0).drawWireframe();
                ofPopMatrix();
            } 
            
            
            if(bOrigin) {    
                ofVec3f p0 = cityMesh.getVerticesPointer()[origin];
                ofVec3f dir = cityMesh.getVerticesPointer()[choice]-cityMesh.getVerticesPointer()[origin];
                float angle = atan2(dir.y, dir.x)*180/M_PI;
                
                ofPushMatrix();
                glTranslatef(p0.x,p0.y,p0.z);
                ofRotate(angle);
                
                tentModel.getMesh(0).drawWireframe();
                ofPopMatrix();
                
            }
            cam.end();
            
            ofSetColor(255);
            
            
            ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, NULL);
            
            
            ofCircle(selected, 5);
            ofDrawBitmapString(ofToString(choice), selected+ofVec2f(5, -25));
        }   break;
        case STATE_SCREEN:
            drawScreen();
            break;
        default:
            break;
    }

        
    switch (state) {
        
        case STATE_TENT:
        case STATE_CITY: {
            ofSetColor(255);
            string msg = string("Using mouse inputs to navigate ('m' to toggle): ") + (cam.getMouseInputEnabled() ? "YES" : "NO");
            msg += "\nfps: " + ofToString(ofGetFrameRate(), 2);
            ofDrawBitmapString(msg, 10, 20);
        } break;
        case STATE_SCREEN:

            break;
        default:
            break;
    }
	
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
            
        
        case 'T':
		case 't':
			state=STATE_TENT;
            cam.enableMouseInput();
			break;
            
        case 'C':
		case 'c':
			state=STATE_CITY;
            cam.enableMouseInput();
			break;
        case 'S':
        case 's':
            state = STATE_SCREEN;
            cam.disableMouseInput();
            if (currentScreen>=overlay.screens.size()) {
                currentScreen = 0;
            }
            break;
    
        case 'P':
        case 'p': 
            bProject = !bProject;
            break;
            
        case 'Q':
        case 'q':
            bScreen = !bScreen;
            break;
        case 'G':
        case 'g':
            cam.begin();
            ofSaveMatrices(ofToDataPath("matrices.txt"));
            cam.end();
            break;
        default:
            break;
    }
    
    switch (state) {
        case STATE_TENT:
            switch(key) {
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
                    
                case 'A':
                case 'a': {
                    
                    int count = 0;
                    for (int i=0; i<selection.size(); i++) {
                        count += selection[i].second ? 1 : 0;
                        
                    }
                    
                    if (count==4) {
                        screen s;
                        
                        vector<ofVec3f> input;
                        for (int i=0; i<selection.size(); i++) {
                            if (selection[i].second) {
                                s.indices.push_back(selection[i].first); 
                                input.push_back(tentMesh.getVerticesPointer()[selection[i].first]);
                            }
                        }

                        findCoordinateSystem(input, s.origin, s.xVec, s.yVec);
                        s.index = selection[0].first;
                        overlay.screens.push_back(s);
                        overlay.save();
                    }
                    
                } break;
                    
                case 'R':
                case 'r': {
                    int ref = choice;
                
                    if (selection[0].second) {
                        ref = selection[0].first;
                    }
                    for (vector<screen>::iterator siter=overlay.screens.begin(); siter!=overlay.screens.end(); siter++) {
                        if (siter->index == ref) {
                            overlay.screens.erase(siter);
                            overlay.save();
                            break;
                        }
                    }
                } break;
                case 'D':
                case 'd': {
                    for (int i=0; i<selection.size(); i++) {
                        selection[i].second = false;
                    }
                } break;
                    
                
                    
            }
            break;
        case STATE_CITY:
            switch(key) { 
            
                case 'L':
                case 'l':
                    bTent=!bTent;
                    bOrigin = false;
                    break;
                case 'O':
                case 'o':
                    bOrigin=!bOrigin;
                    origin = choice;
                    break;
                case 'A': 
                case 'a': 
                    if (bOrigin) {
                        tent t;
                        t.indices.push_back(origin) ;
                        t.indices.push_back(choice) ;
                        t.vertices.push_back(cityMesh.getVerticesPointer()[origin]);
                        t.vertices.push_back(cityMesh.getVerticesPointer()[choice]);
                        overlay.tents.push_back(t);
                        bOrigin = false;
                        overlay.save();
                        
                    }
                    break;
                case 'R':
                case 'r':  {  
                    int ref = choice;
                    
                    for (vector<tent >::iterator titer=overlay.tents.begin(); titer!=overlay.tents.end(); titer++) {
                        if (titer->indices[0] == ref) {
                            overlay.tents.erase(titer);
                            overlay.save();
                            break;
                        }
                    }
                }   break;  
                    
                
                    
                default:
                 break;
            }
            break;
            
        case STATE_SCREEN: 
            switch (key) {
                case 'R':
                case 'r':
                    if (!overlay.screens.empty()) {
                        screen s = overlay.screens[currentScreen];
                        
                        ofVec3f p0 = tentMesh.getVerticesPointer()[s.indices[0]];
                        ofVec3f p1 = tentMesh.getVerticesPointer()[s.indices[3]];
                        
                        float proj = s.xVec.dot(p1-p0);
                        s.origin = p0+s.xVec*proj; 
                        
                        overlay.screens[currentScreen] = s;
                        
                        overlay.save();
                    }
                    break;
                case OF_KEY_LEFT:
                    currentScreen = (currentScreen-1+overlay.screens.size()) % overlay.screens.size();
                    break;
                case OF_KEY_RIGHT:
                    currentScreen = (currentScreen+1) % overlay.screens.size();
                    break;
                    
                default:
                    break;
            }
            break;
           
        default:
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
    if (state == STATE_SCREEN && !overlay.screens.empty()) {
        screen s = overlay.screens[currentScreen];
        
        ofVec3f p0 = tentMesh.getVerticesPointer()[s.indices[0]];
        ofVec3f p1 = tentMesh.getVerticesPointer()[s.indices[3]];
        
        float proj = s.xVec.dot(p1-p0);
        ofVec3f topLeft = p0+s.xVec*proj; 
        
        
        s.origin = topLeft+x/scale*s.xVec+y/scale*s.yVec;
        
        overlay.screens[currentScreen] = s;
        
        overlay.save();
    }
    
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
