#include "testApp.h"
#include "ofxProCamToolkit.h"
#include "ofxXmlSettings.h"
#include "Poco/String.h"

#define PIXEL_PER_CENTIMETER 60
#define WORD_DURATION 2000 //ms
#define MESSAGE_REPETITION 3
#define PORT 12345

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
    
    printf( "The number of bytes that comprise the widechar "
           "string is %i\n", length );
    
    return wstring(buffer,length);
}


string WStringToString(const wstring& s)
{
    
//    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
//    cout << "Current LC_CTYPE is " << local << endl;
//    
    char buffer[100];
    int length = wcstombs ( buffer, s.c_str(), 100);

    printf( "The number of bytes that comprise the multibyte "
           "string is %i\n", length );
    
    return string(buffer,length);
    
    
}

//--------------------------------------------------------------
void testApp::setup(){
    
    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
 //   cout << "Current LC_CTYPE is " << local << endl;
    
    receiver.setup( PORT );
    
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
    
    font.loadFont("Guttman Hatzvi-Normal.ttf", 50,true,true);
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
//	glEnable(GL_DEPTH_TEST);
	
	// this sets the camera's distance from the object
	
    // we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();

    
    cityModel.loadModel("base.obj");
    //cityModel.loadModel("base.3ds");
    cityMesh = cityModel.getMesh(0);
    tentModel.loadModel("tent.obj");
	tentMesh = tentModel.getMesh(0);
    
    cout<< "numIndices: " << cityMesh.getNumIndices() << endl;
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofBackground(0);
    
    
    
    bProject = true;
    bScreen = false;
    
    scale = PIXEL_PER_CENTIMETER;
   
    
    load();
    ofLoadMatrices(ofToDataPath("matrices.txt"),projMat,mvMat);
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
	glEnable(GL_DEPTH_TEST);
    
    //some model / light stuff
    glShadeModel(GL_SMOOTH);
    light.enable();
    ofEnableSeparateSpecularLight();
    
    bEasyCam = false;
    
    //cam.setDistance();
    
    
//    sendMessage(4, WStringToString(L"יתרבח קדצ שרוד םעה"));
//    sendMessage(4, WStringToString(L"ןוטלשל טייטש"));
    
}

void testApp::sendMessage(int tent,string str) {
    if (tent<tents.size()) {
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
        int current = (ofGetElapsedTimeMillis()-miter->startTime) / WORD_DURATION; // WORD_DURATION ms for each word
        if (current<miter->words.size()*MESSAGE_REPETITION) {
            miter->current = current  % miter->words.size();
            float phase = (float)((ofGetElapsedTimeMillis()-miter->startTime) % WORD_DURATION) / WORD_DURATION;
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
    
    ofSetColor(255);
    
    glEnable(GL_DEPTH_TEST);
    
    cityMesh.draw();
    
    
    
    for (vector<tent>::iterator iter=tents.begin(); iter!=tents.end(); iter++) {
        ofVec3f p0 = cityMesh.getVerticesPointer()[iter->first];
        ofVec3f dir = cityMesh.getVerticesPointer()[iter->second]-cityMesh.getVerticesPointer()[iter->first];
        
        ofPushMatrix();
        glTranslatef(p0.x,p0.y,p0.z);
        ofRotate(atan2(dir.y, dir.x)*180/PI);
        
        ofSetColor(100);
        
        
        tentModel.getMesh(0).drawFaces();
        
       
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
    
    
    glDisable(GL_DEPTH_TEST);
    
    if (bProject) {
        
        for (vector<message>::iterator miter=display.begin(); miter!=display.end(); miter++) {
            vector<tent>::iterator iter=tents.begin()+miter->tent;
            
            ofPushMatrix();
            glMultMatrixf(iter->glMat.getPtr());
            
            for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
                ofPushMatrix();
                glMultMatrixf(siter->glMat.getPtr());
                
                ofTranslate(rects.front().x,rects.front().y);            
                ofFill();
                ofSetColor(0,0,0,miter->alpha*255);
                
                
                wstring word(StringToWString(miter->words[miter->current]));
                reverse(word.begin(),word.end());
                font.drawString(word, (rects.front().width-font.stringWidth(word))/2, font.stringHeight(word)); 
                
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
    
    if (bProject && !lastMessage.empty()) {
        ofSetColor(255);
        font.drawString(StringToWString(lastMessage),50,50);
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
            
        case 'Q':
        case 'q':
            bScreen = !bScreen;
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
