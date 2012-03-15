#include "testApp.h"
#include "ofxProCamToolkit.h"
#include "ofxRKProCamToolkit.h"
#include "ofxXmlSettings.h"

#define PIXEL_PER_CENTIMETER 60

enum {
    STATE_TENT,
    STATE_SCREEN,
    STATE_MODEL
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
    cityModel.loadModel("BASE.obj",true);
    cityMesh = cityModel.getMesh(0);
    tentModel.loadModel("tent_low.obj",true);
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
    
    load();
}

//--------------------------------------------------------------
void testApp::update(){
    
}





void testApp::drawTent() {
    ofSetColor(100);
    
	cam.begin();
    //cityModel.drawWireframe();
    tentMesh.drawWireframe();
    
    // check to see if anything is selected
    // draw hover point magenta
    
    ofMesh imageMesh = getProjectedMesh(tentMesh);
    
    
    if (bScreen) {
        ofSetColor(0, 0, 255,100);
        
        for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
            ofBeginShape();
            
            ofVec3f pnt = siter->origin;
            ofVec3f wVec = siter->xVec*ofGetWidth()/scale;
            ofVec3f hVec = siter->yVec*ofGetHeight()/scale;
            
            ofVertex(pnt);
            pnt+=wVec;
            ofVertex(pnt);
            pnt+=hVec;
            ofVertex(pnt);
            pnt-=wVec;
            ofVertex(pnt);
            ofEndShape(true);
        }
    }
    
	cam.end();
    
    if (bProject) {
                
        ofMatrix4x4 projMat;    
        cam.begin();
        glGetFloatv(GL_PROJECTION_MATRIX, projMat.getPtr());
        cam.end();
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(projMat.getPtr());
        
        for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
        
            

            ofMatrix4x4 mvMat;
                        
            cam.begin();
            
            ofPushMatrix();
            glMultMatrixf(siter->glMat.getPtr());
            glGetFloatv(GL_MODELVIEW_MATRIX,  mvMat.getPtr());
            
            ofPopMatrix();
            cam.end();
            
            glPushMatrix();
            glLoadMatrixf(mvMat.getPtr());
            
            ofFill();
            ///ofRect(0, 0, 1, 1);
            ofSetColor(255, 0, 0);
            ofLine(0, 0, 200, 0);
            ofSetColor(0, 255, 0);
            ofLine(0, 0, 0, 200);
            
            ofTranslate(rects.front().x,rects.front().y);            
            ofFill();
            ofSetColor(255);
            
            //ofCircle(ofGetWidth()/2, ofGetHeight()/2, ofGetHeight()/2);   
            wstring message = L"יתרבח קדצ שרוד םעה";
            font.drawString(message, 0, font.stringHeight(message)); // 
            
            
            glPopMatrix();
        }
        
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        
    }

    
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
}

void testApp::drawScreen() {
    
    
    
    if (!screens.empty()) {
        screen s = screens[currentScreen];
        
        
        
        ofSetColor(0, 0, 255,100);
        
        ofBeginShape();
        for (vector<int>::iterator viter=s.indices.begin(); viter!=s.indices.end(); viter++) {
            
            ofVec3f vec = tentMesh.getVerticesPointer()[*viter]-s.origin;
            ofVec3f pos;
            pos.z = 0;
            pos.x = s.xVec.dot(vec)*scale;
            pos.y = s.yVec.dot(vec)*scale;
            ofVertex(pos);
        }
        ofEndShape(true);
    }
    
    ofSetColor(0, 255, 0,100);
    ofFill; 
    for (vector<ofRectangle>::iterator iter = rects.begin(); iter!=rects.end(); iter++) {
        ofRect(*iter);
    }
    
    if (ofGetMousePressed()) {
        ofRect(downPos, ofGetMouseX()-downPos.x, ofGetMouseY()-downPos.y);
    }

}

void testApp::drawModel() {
    ofSetColor(100);
    
	cam.begin();
    //cityModel.drawWireframe();
   // cityMesh.drawWireframe();
    cityMesh.draw();
    
    // check to see if anything is selected
    // draw hover point magenta
    
    ofMesh imageMesh = getProjectedMesh(cityMesh);
    
    
    
	cam.end();
    
    
    
    ofSetColor(255);
    
    
    ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, NULL);
    
    
    ofCircle(selected, 5);
    ofDrawBitmapString(ofToString(choice), selected+ofVec2f(5, -25));
    
    
    cam.begin();
    
    for (vector<tent>::iterator iter=tents.begin(); iter!=tents.end(); iter++) {
        ofVec3f p0 = cityMesh.getVerticesPointer()[iter->first];
        ofVec3f dir = cityMesh.getVerticesPointer()[iter->second]-cityMesh.getVerticesPointer()[iter->first];
        
        ofPushMatrix();
        glTranslatef(p0.x,p0.y,p0.z);
        ofRotate(atan2(dir.y, dir.x)*180/M_PI);
        
        ofSetColor(255);
        tentModel.getMesh(0).draw();
        
       
        if (bScreen) {
            ofSetColor(0, 0, 255,100);
            for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
                ofBeginShape();
                
                ofVec3f pnt = siter->origin;
                ofVec3f wVec = siter->xVec*ofGetWidth()/scale;
                ofVec3f hVec = siter->yVec*ofGetHeight()/scale;
                
                ofVertex(pnt);
                pnt+=wVec;
                ofVertex(pnt);
                pnt+=hVec;
                ofVertex(pnt);
                pnt-=wVec;
                ofVertex(pnt);
                ofEndShape(true);
            }

        }

       
        ofPopMatrix();
    }
    
    
    
    

    
    
    
    ofSetColor(255);
    
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
    
    if (bProject && !rects.empty()) {
        
        vector<wstring> words;
        words.push_back(L"םעה");
        words.push_back(L"שרוד");
        words.push_back(L"קדצ");
        words.push_back(L"יתרבח");
        
        
        ofMatrix4x4 projMat;  
        ofMatrix4x4 mvMat;
        cam.begin();
        glGetFloatv(GL_PROJECTION_MATRIX, projMat.getPtr());
        glGetFloatv(GL_MODELVIEW_MATRIX,  mvMat.getPtr());
        cam.end();
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(projMat.getPtr());
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(mvMat.getPtr());
        
        for (vector<tent>::iterator iter=tents.begin(); iter!=tents.end(); iter++) {
            ofPushMatrix();
            glMultMatrixf(iter->glMat.getPtr());
            
            for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
                ofPushMatrix();
                glMultMatrixf(siter->glMat.getPtr());
        
                ofTranslate(rects.front().x,rects.front().y);            
                ofFill();
                ofSetColor(0);
                
                int dist = distance(screens.begin(),siter);
                    
                if (dist<words.size()) {
                    wstring word = words[dist];
                    font.drawString(word, (rects.front().width-font.stringWidth(word))/2, font.stringHeight(word)); 
                }
                
                 
                
                glPopMatrix();
            }
            glPopMatrix();
        }
        glPopMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
    switch (state) {
        case STATE_TENT:
            drawTent();
            break;
        case STATE_MODEL:
            drawModel();
            break;
        case STATE_SCREEN:
            drawScreen();
            break;
        default:
            break;
    }

        
    switch (state) {
        
        case STATE_TENT:
        case STATE_MODEL: {
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


void readVertex(ofxXmlSettings &xml,ofVec3f &vertex,int i) {
    vertex = ofVec3f(xml.getAttribute("vertex", "x", 0.0f,i),xml.getAttribute("vertex", "y", 0.0f,i),xml.getAttribute("vertex", "z", 0.0f,i));
}

void writeVertex(ofxXmlSettings &xml,ofVec3f &vertex,int i) {
    xml.addTag("vertex");
    xml.addAttribute("vertex", "x", vertex.x,i);
    xml.addAttribute("vertex", "y", vertex.y,i);
    xml.addAttribute("vertex", "z", vertex.z,i);
}


void testApp::load() {
    ofxXmlSettings xml;
    xml.loadFile("settings.xml");
    xml.pushTag("city");
    
    for (int i=0;i<xml.getNumTags("screen"); i++) {
        
        screen s;
        s.index = xml.getAttribute("screen", "index",0, i);
        
        xml.pushTag("screen",i);
       
        for (int j=0;j<xml.getNumTags("index");j++) {
            s.indices.push_back(xml.getAttribute("index", "value", 0,j));
            
        }
       
        readVertex(xml, s.origin,0);
        readVertex(xml,s.xVec, 1);
        readVertex(xml,s.yVec, 2);
        
        screens.push_back(s);
        xml.popTag();
    }
    
    for (int i=0;i<xml.getNumTags("tent");i++) {
        tent t;
        t.first = xml.getAttribute("tent", "first",0,i);
        t.second = xml.getAttribute("tent", "second",0,i);
        tents.push_back(t);
        
    }
    
    for (int i=0;i<xml.getNumTags("rect");i++) {
        rects.push_back(ofRectangle(xml.getAttribute("rect", "x",0.0f,i),xml.getAttribute("rect", "y",0.0f,i),xml.getAttribute("rect", "w",0.0f,i),xml.getAttribute("rect", "h",0.0f,i)));
        
    }
    
    xml.popTag();
    
    updateMatices();
}

void testApp::save() {
    ofxXmlSettings xml;
    
    xml.addTag("city");
    xml.pushTag("city");
    
    for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
        xml.addTag("screen");
        xml.addAttribute("screen", "index", siter->index,0);
        xml.pushTag("screen",distance(screens.begin(), siter));
        
        for (vector<int>::iterator iter=siter->indices.begin();iter!=siter->indices.end();iter++) {
            xml.addTag("index");
            xml.addAttribute("index", "value", *iter,distance(siter->indices.begin(), iter));
        }
        
        writeVertex(xml, siter->origin,0);
        writeVertex(xml, siter->xVec,1);
        writeVertex(xml, siter->yVec,2);
        
        
        xml.popTag();
    }
    
    for (vector<tent >::iterator titer=tents.begin(); titer!=tents.end(); titer++) {
        xml.addTag("tent");
        xml.addAttribute("tent", "first", titer->first,distance(tents.begin(), titer));
        xml.addAttribute("tent", "second", titer->second,distance(tents.begin(), titer));
    }
    
    for (vector<ofRectangle>::iterator riter=rects.begin();riter!=rects.end();riter++) {
        xml.addTag("rect");
        xml.addAttribute("rect", "x", riter->x,distance(rects.begin(), riter));
        xml.addAttribute("rect", "y", riter->y,distance(rects.begin(), riter));
        xml.addAttribute("rect", "w", riter->width,distance(rects.begin(), riter));
        xml.addAttribute("rect", "h", riter->height,distance(rects.begin(), riter));
    }
    
    xml.popTag();
    
    xml.saveFile("settings.xml");
    
    updateMatices();
    
}

void testApp::log() {
    
    for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
        cout << siter->index << endl;
        cout << "indices: ";
        for (vector<int>::iterator iter=siter->indices.begin();iter!=siter->indices.end();iter++) {
            cout << *iter << " ";
        }
        cout << endl;
//        cout << "vertices: " ;
//
//        for (vector<ofVec3f>::iterator viter=siter->vertices.begin();viter!=siter->vertices.end();viter++) {
//            cout << viter->x << " " << viter->y << " " << viter->z << endl;
//        }
    }
    
    for (vector<tent >::iterator titer=tents.begin(); titer!=tents.end(); titer++) {
        cout << "tent: " << titer->first << " " << titer->second << endl;
    }
    
   

}

void testApp::updateMatices() {
    for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
        ofMatrix4x4 trans = ofMatrix4x4::getTransposedOf(ofMatrix4x4::newTranslationMatrix(siter->origin));
        // double rotation - align x axis and align new y yaxis
        ofMatrix4x4 rot = ofMatrix4x4::newRotationMatrix(siter->xVec,ofVec3f(1.0f,0.0f,0.0f));
        rot.postMult(ofMatrix4x4::newRotationMatrix(siter->yVec*rot,ofVec3f(0.0f,1.0f,0.0f)));
        ofMatrix4x4 scl = ofMatrix4x4::newScaleMatrix(1.0f/scale,1.0f/scale,1.0f); 
        siter->glMat = ofMatrix4x4::getTransposedOf(trans*rot*scl);
    }
    
    for (vector<tent>::iterator iter=tents.begin(); iter!=tents.end(); iter++) {
        ofMatrix4x4 trans = ofMatrix4x4::getTransposedOf(ofMatrix4x4::newTranslationMatrix(cityMesh.getVerticesPointer()[iter->first]));
        
        ofMatrix4x4 rot = ofMatrix4x4::newRotationMatrix(cityMesh.getVerticesPointer()[iter->second]-cityMesh.getVerticesPointer()[iter->first],ofVec3f(1.0f,0.0f,0.0f));
        
        iter->glMat = ofMatrix4x4::getTransposedOf(trans*rot);
        
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
			state=STATE_MODEL;
            cam.enableMouseInput();
			break;
        case 'S':
        case 's':
            state = STATE_SCREEN;
            cam.disableMouseInput();
            if (currentScreen>=screens.size()) {
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
                    
                    if (count>=3) {
                        screen s;
                        
                        vector<ofVec3f> input;
                        for (int i=0; i<selection.size(); i++) {
                            if (selection[i].second) {
                                s.indices.push_back(selection[i].first); 
                                input.push_back(tentMesh.getVerticesPointer()[selection[i].first]);
                            }
                        }

                        vector<ofVec3f> bb = findBoundingBox(input);
                        screenCoordinateSystem(bb, s.origin, s.xVec, s.yVec);
                        //s.vertices = findScreenCorners(bb, ofGetWidth(), ofGetHeight(), scale);
                        s.index = selection[0].first;
                        screens.push_back(s);
                        save();
                    }
                    
                } break;
                    
                case 'R':
                case 'r': {
                    int ref = choice;
                
                    if (selection[0].second) {
                        ref = selection[0].first;
                    }
                    for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
                        if (siter->index == ref) {
                            screens.erase(siter);
                            save();
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
        case STATE_MODEL:
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
                        t.first = origin;
                        t.second = choice;
                        tents.push_back(t);
                        bOrigin = false;
                        save();
                        
                    }
                    break;
                case 'R':
                case 'r':  {  
                    int ref = choice;
                    
                    for (vector<tent >::iterator titer=tents.begin(); titer!=tents.end(); titer++) {
                        if (titer->first == ref) {
                            tents.erase(titer);
                            save();
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
                    for (vector<ofRectangle>::iterator iter = rects.begin(); iter!=rects.end(); iter++) {
                        if (iter->inside(ofVec2f(ofGetMouseX(),ofGetMouseY()))) {
                            rects.erase(iter);
                            save();
                            break; 
                        }                    
                    }
                    break;
                case OF_KEY_LEFT:
                    currentScreen = (currentScreen-1+screens.size()) % screens.size();
                    break;
                case OF_KEY_RIGHT:
                    currentScreen = (currentScreen+1) % screens.size();
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
    switch (state) {
        case STATE_SCREEN:
            downPos = ofVec2f(x,y);
            break;
            
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    switch (state) {
        case STATE_SCREEN:
            rects.push_back(ofRectangle(downPos.x,downPos.y,x-downPos.x,y-downPos.y));
            save();
            break;
            
        default:
            break;
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
