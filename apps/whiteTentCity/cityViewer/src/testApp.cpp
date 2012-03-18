#include "testApp.h"
#include "ofxProCamToolkit.h"
#include "ofxRKProCamToolkit.h"
#include "ofxXmlSettings.h"
#include "ofxStringUtils.h"





//--------------------------------------------------------------
void testApp::setup(){
    
    SetLocale("he_IL.UTF-8");
   
    ofxXmlSettings xml;
    xml.loadFile("viewer.xml");
    xml.pushTag("viewer");
    
    
    receiver.setup( xml.getAttribute("osc", "port", 12345));
    
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
    
    font.loadFont(xml.getAttribute("font", "filename", "Guttman Hatzvi-Normal.ttf"), xml.getAttribute("font", "size", 50),true,true);
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
//	glEnable(GL_DEPTH_TEST);
	
	// this sets the camera's distance from the object
	
    // we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();

    ofxAssimpModelLoader model;	
    
    
    model.loadModel(xml.getAttribute("model", "city", ""));
    cityMesh = model.getMesh(0);
    model.loadModel(xml.getAttribute("model", "tent", ""));
	tentMesh = model.getMesh(0);
    

    wordDuration = xml.getAttribute("animation", "wordDuration", 2000);
    messageRepetition = xml.getAttribute("animation", "messageRepetition", 3);
    
    xml.popTag();
    
    cout<< "numIndices: " << cityMesh.getNumIndices() << endl;
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground(0);
    
    
    
    bProject = true;
    bDebug = false;
    
   
   
    
    overlay.load();
    ofLoadMatrices(ofToDataPath("matrices.txt"),projMat,mvMat);
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
	glEnable(GL_DEPTH_TEST);
    
    //some model / light stuff
    glShadeModel(GL_SMOOTH);
    light.enable();
    ofEnableSeparateSpecularLight();
    
    bEasyCam = false;
    
    glEnable(GL_NORMALIZE);
    
    //cam.setDistance();
    
    
//    sendMessage(4, WStringToString(L"יתרבח קדצ שרוד םעה"));
//    sendMessage(4, WStringToString(L"ןוטלשל טייטש"));
    
}



//--------------------------------------------------------------
void testApp::update(){

    
    while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
		// check for mouse moved message
		if ( m.getAddress() == "/sendMessage" )
		{
			
            int tent = m.getArgAsInt32(0);
            
            lastMessage = m.getArgAsString(1);
            message msg;
            msg.words = ofSplitString(m.getArgAsString(1), " ");
            msg.startTime = ofGetElapsedTimeMillis();
            messages[tent] = msg;
            cout << "received: " << tent << " " << lastMessage << endl;
                
            
		}
    }
    
    vector<int> messagesDone;
    
    
    for (map<int,message>::iterator miter=messages.begin(); miter!=messages.end(); miter++) {
        int current = (ofGetElapsedTimeMillis()-miter->second.startTime) / wordDuration; // WORD_DURATION ms for each word
        if (current<miter->second.words.size()*messageRepetition) {
            miter->second.current = current  % miter->second.words.size();
            float phase = (float)((ofGetElapsedTimeMillis()-miter->second.startTime) % wordDuration) / wordDuration;
            miter->second.alpha = ofMap(phase, 0, 0.1, 0, 1,true)-ofMap(phase, 0.9, 1, 0, 1,true);
            
        } else {
            messagesDone.push_back(miter->first);
        }
    }
    
    
    for (vector<int>::iterator iter=messagesDone.begin();iter!=messagesDone.end();iter++) {
        messages.erase(*iter); 
    }    
    
}





void testApp::draw() {
    
    glMatrixMode(GL_PROJECTION);
    ofPushMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    ofPushMatrix();
   
    
    if (bEasyCam) {
        cam.begin();
    } else {
        ofApplyMatrices(projMat, mvMat);
    }
    
   
    glEnable(GL_DEPTH_TEST);
    
    ofSetColor(50);
    cityMesh.drawFaces();
    ofSetColor(100);
    for (vector<tent>::iterator iter=overlay.tents.begin(); iter!=overlay.tents.end(); iter++) {
        ofPushMatrix();
        glMultMatrixf(iter->glMat.getPtr());
        
        tentMesh.drawFaces();
        
        ofPopMatrix();
    }
    
    
    if (bProject) {
        glDisable(GL_DEPTH_TEST);
        ofFill();
        for (vector<tent>::iterator iter=overlay.tents.begin(); iter!=overlay.tents.end(); iter++) {
        
            ofPushMatrix();
            glMultMatrixf(iter->glMat.getPtr());
            
                
            vector<screen>::iterator siter;
            
            map<int,message>::iterator miter=messages.find(iter->id);
            if (miter!=messages.end()) {
                wstring word(StringToWString(miter->second.words[miter->second.current]));
                reverse(word.begin(),word.end());
                
                for (int i=0; i<4; i++) {
                    siter=overlay.screens.begin()+i;
                    ofPushMatrix();
                    glMultMatrixf(siter->glMat.getPtr());
                    ofSetColor(0,0,0,miter->second.alpha*255);
                    font.drawString(word, - font.stringWidth(word)/2, font.stringHeight(word)/2); 
                    ofPopMatrix();
                }
            }
            
           
            
            string s(ofToString(iter->id));
            wstring word=L"101";
            word.assign(s.begin(), s.end());
            
            siter=overlay.screens.begin()+4;
            ofPushMatrix();
            glMultMatrixf(siter->glMat.getPtr());
            ofSetColor(0);
            font.drawString(word, -font.stringWidth(word)/2, font.stringHeight(word)/2); 
            ofPopMatrix();
            
            ofPopMatrix();
        }
    }

    
             
                
                
                 
    if (bEasyCam) {
        cam.end();
    }

    glMatrixMode(GL_PROJECTION);
    ofPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    ofPopMatrix();
    
    if (bDebug && !lastMessage.empty()) {
        ofSetColor(255);
        font.drawString(StringToWString(lastMessage),50,50);
    }
}





//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
					
		case 'F':
		case 'f':
			ofToggleFullscreen();
			break;
        case 'C':
		case 'c':
			bEasyCam=!bEasyCam;
			break;
    
    
    
        case 'P':
        case 'p': 
            bProject = !bProject;
            break;
            
        case 'D':
        case 'd':
            bDebug = !bDebug;
            break;
        case 'G':
        case 'g':
            if (bEasyCam) {
                cam.begin();
                ofSaveMatrices(ofToDataPath("matrices.txt"));
                cam.end();
            }
            break;
            
        case 'L':
        case 'l':
            
            ofLoadMatrices(ofToDataPath("matrices.txt"),projMat,mvMat);
            
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
