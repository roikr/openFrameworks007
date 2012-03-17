#include "testApp.h"
#include "ofxProCamToolkit.h"
#include "ofxRKProCamToolkit.h"
#include "ofxXmlSettings.h"
#include "Poco/String.h"

/*
//--------------------------------------------------
vector <wstring> ofSplitWString(const wstring & source, const string & delimiter, bool ignoreEmpty = false, bool trim = false);

vector <wstring> ofSplitWString(const wstring & source, const string & delimiter, bool ignoreEmpty, bool trim) {
	vector<wstring> result;
	if (delimiter.empty()) {
		result.push_back(source);
		return result;
	}
	wstring::const_iterator substart = source.begin(), subend;
	while (true) {
		subend = search(substart, source.end(), delimiter.begin(), delimiter.end());
		wstring sub(substart, subend);
		if(trim) {
			Poco::trimInPlace(sub);
		}
		if (!ignoreEmpty || !sub.empty()) {
			result.push_back(sub);
		}
		if (subend == source.end()) {
			break;
		}
		substart = subend + delimiter.size();
	}
	return result;
}
 
*/

wstring StringToWString(const string& s);
string WStringToString(const wstring& s);

wstring StringToWString(const string& s)
{
//    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
//    cout << "Current LC_CTYPE is " << local << endl;
//    
    wchar_t buffer[100];
    int length = mbstowcs(buffer, s.c_str(), 100);
    
//    printf( "The number of bytes that comprise the widechar "
//           "string is %i\n", length );
    
    return wstring(buffer,length);
}


string WStringToString(const wstring& s)
{
    
//    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
//    cout << "Current LC_CTYPE is " << local << endl;
//    
    char buffer[100];
    int length = wcstombs ( buffer, s.c_str(), 100);

//    printf( "The number of bytes that comprise the multibyte "
//           "string is %i\n", length );
//    
    return string(buffer,length);
    
    
}

//--------------------------------------------------------------
void testApp::setup(){
    
    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
    cout << "Current LC_CTYPE is " << local << endl;
    
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
    //cityModel.loadModel("base.3ds");
    cityMesh = model.getMesh(0);
    model.loadModel(xml.getAttribute("model", "tent", ""));
	tentMesh = model.getMesh(0);
    

    wordDuration = xml.getAttribute("animation", "wordDuration", 2000);
    messageRepetition = xml.getAttribute("animation", "messageRepetition", 3);
    
    
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

void testApp::sendMessage(int tent,string str) {
    if (tent<overlay.tents.size()) {
        message m;
        m.tent = tent;
        
        vector<string> list = ofSplitString(str, " ");
        
//        for (vector<string>::reverse_iterator iter=list.rbegin(); iter!=list.rend(); iter++) {
//            m.words.push_back(*iter);
//        }
//        for (vector<string>::iterator iter=list.begin(); iter!=list.end(); iter++) {
//            string message = *iter;
//            reverse(message.begin(),message.end());
//            m.words.push_back(message);
//        }
        
        m.words = list;
        
        queue.push_back(m);
    }
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
			lastMessage = m.getArgAsString(1);
            sendMessage(m.getArgAsInt32(0),lastMessage );
		}
    }
    
    vector<message> newDisplay;
    
    
    for (vector<message>::iterator miter=display.begin(); miter!=display.end(); miter++) {
        int current = (ofGetElapsedTimeMillis()-miter->startTime) / wordDuration; // WORD_DURATION ms for each word
        if (current<miter->words.size()*messageRepetition) {
            miter->current = current  % miter->words.size();
            float phase = (float)((ofGetElapsedTimeMillis()-miter->startTime) % wordDuration) / wordDuration;
            miter->alpha = ofMap(phase, 0, 0.1, 0, 1,true)-ofMap(phase, 0.9, 1, 0, 1,true);
            newDisplay.push_back(*miter);
        } 
    }
    
    
    if (newDisplay.size()!=display.size()) {
        display = newDisplay;
    }
    
    vector<message> newQueue;
    
    for (vector<message>::iterator miter=queue.begin(); miter!=queue.end(); miter++) {
        vector<message>::iterator diter;
        for (diter=display.begin(); diter!=display.end() && miter->tent != diter->tent; diter++);
        if (diter==display.end()) {
            miter->current = 0 ;
            miter->alpha = 0;
            miter->startTime = ofGetElapsedTimeMillis();
            display.push_back(*miter);
        } else {
            newQueue.push_back(*miter);
        }
    }

    
    if (newQueue.size()!=queue.size()) {
        queue = newQueue;
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
        for (vector<message>::iterator miter=display.begin(); miter!=display.end(); miter++) {
            vector<tent>::iterator iter=overlay.tents.begin()+miter->tent;
            
            ofPushMatrix();
            glMultMatrixf(iter->glMat.getPtr());
            
            for (vector<screen>::iterator siter=overlay.screens.begin(); siter!=overlay.screens.end(); siter++) {
                
                ofPushMatrix();
                glMultMatrixf(siter->glMat.getPtr());
                
                ofSetColor(0,0,0,miter->alpha*255);
               
                wstring word(StringToWString(miter->words[miter->current]));
                reverse(word.begin(),word.end());
                font.drawString(word, (WORD_LENGTH * PIXEL_PER_CENTIMETER - font.stringWidth(word))/2, font.stringHeight(word)); 
                ofPopMatrix();
            }
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
