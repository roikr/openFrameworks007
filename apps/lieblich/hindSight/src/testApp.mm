#include "testApp.h"





//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	// hacking of ofxiPhoneAppDelegate.mm - flip frame buffer width and height
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0) {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    } else {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    }
    //
    
    action_url = "http://192.168.10.97:8888/postImage.of";
	ofAddListener(upload.newResponseEvent,this,&testApp::newResponse);
	upload.start();
    counter = 0;
    
    ofxRegisterStillCameraNotification(this);
    ofxRegisterVolumeButtonsNotification(this);
    buttons.start();
    cout << ofGetWidth() << "x" << ofGetHeight() << endl;
    offscreen.setup(ofGetWidth(),ofGetHeight());
    pixels.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
    
   
    
//    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
//        mat.translate(64, 0, 0);
//        mat.scale(5.0/6.0, 5.0/6.0, 1.0);
//    }
    
    ofEnableAlphaBlending();
    
    sliderPrefs prefs;

       
    ofDirectory dir;
    dir.allowExt("png");
    dir.listDir(ofToDataPath("masks"));
    for (int i=0; i<dir.numFiles(); i++) {
        cout << "loading " << dir.getName(i) << endl;
		card c;
        c.state = STATE_MEMORY_UNLOADED;
        c.filename = dir.getPath(i);
		cards.push_back(c);
		prefs.pages.push_back(ofPoint(0,ofGetHeight()*i));
		
	}
    
	prefs.direction = SLIDER_VERTICAL;
	prefs.bCyclic = true;
	prefs.lastPageSize = ofPoint(0,ofGetHeight());
	slider.setup(1,prefs);
    
    cam.preview();
    
    startThread();
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
   
    cam.update();
    slider.update();
    
    
    int j=0;
    int i=slider.getCurrentPage()-2;
    
    for (;j<5;i++,j++) {
        int k = (i+cards.size())%cards.size();
        if (cards[k].state==STATE_MEMORY_LOADED) {
            cards[k].mask.setUseTexture(true);
            cards[k].mask.update();
            cards[k].state=STATE_TEXTURE_LOADED;
             cout << "load texture " << k << endl;
            
        }
    }
    for (;j<cards.size();i++,j++) {
        int k = (i+cards.size())%cards.size();
        if (cards[k].state==STATE_TEXTURE_LOADED) {
            cards[k].mask.getTextureReference().clear();
            cards[k].state = STATE_TEXTURE_UNLOADED;
             cout << "unload texture " << k << endl;
        }
    }
    
    
    
}

void testApp::threadedFunction() {
    
    while( isThreadRunning() != 0 ){
        
        int j=0;
        int i=slider.getCurrentPage()-2;
        
        for (;j<5;i++,j++) {
            int k = (i+cards.size())%cards.size();
            if (cards[k].state==STATE_MEMORY_UNLOADED) {
                cards[k].mask.setUseTexture(false);
                cards[k].mask.loadImage(cards[k].filename);
                cards[k].state=STATE_MEMORY_LOADED;
                cout << "load memory " << k << endl;
            }
        }
        for (;j<cards.size();i++,j++) {
            int k = (i+cards.size())%cards.size();
            if (cards[k].state==STATE_TEXTURE_UNLOADED) {
                cards[k].mask.clear();
                cards[k].state=STATE_MEMORY_UNLOADED;
                cout << "unload memory " << k << endl;
            }
        }
        ofSleepMillis(10);
        
        
    }
    
    
    
}

void testApp::drawCard(card &c) {
    if (photo.isAllocated()) {
        photo.draw(0, 0);
    } else {
        if (c.state==STATE_TEXTURE_LOADED) {
            c.mask.draw(0, 0);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
    
    if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
//        float tw = imageRect.width/imageRect.height*cam.getHeight()/cam.getWidth();
//        cam.draw(imageRect, ofRectangle((1-tw)/2,0,tw,1));
        
        cam.draw(ofRectangle(0, 0, cam.getWidth(), cam.getHeight()), ofRectangle(1, 1, -1, -1));
    }
    
    
    ofPushMatrix();
	slider.transform();
	
	
    ofPushMatrix();
    ofTranslate(0, -ofGetHeight(), 0);
    drawCard(*(cards.end()-1));
    ofPopMatrix();
    
    for (vector<card>::iterator iter=cards.begin(); iter!=cards.end(); iter++) {
        
        ofPushMatrix();
        ofTranslate(0, distance(cards.begin(), iter) *ofGetHeight(), 0);
        drawCard(*iter);
        ofPopMatrix();
       
    }
    
    ofPushMatrix();
    ofTranslate(0, ofGetHeight()*cards.size(), 0);
    drawCard(*(cards.begin()));
    ofPopMatrix();
    
    ofPopMatrix();
    
    
}

void testApp::exit() {
    ofxUnregisterVolumeButtonsNotification(this);
     buttons.stop();
    ofxUnregisterStillCameraNotification(this);
       
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    slider.touchDown(touch.x, touch.y,touch.id);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
   slider.touchMoved(touch.x, touch.y,touch.id);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    slider.touchUp(touch.x, touch.y,touch.id);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
//    mailStruct m;
//    m.subject = "Hindsight, first prototype";
//    for (vector<card>::iterator iter=cards.begin(); iter!=cards.end(); iter++) {
//        if (iter->photo.isAllocated()) {
//            m.attachments.push_back(attachmentStruct(iter->buf,"image/jpeg", "CARD_"+ofToString(distance(cards.begin(), iter))+".jpg"));
//        }
//    }
//    mail.sendMail(m);
    

    upload.upload(ofxFile("card_"+ofToString(counter)+".jpg", ofxiPhoneGetDocumentsDirectory()+"temp.jpg",action_url));
	counter++;
    
 
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

void testApp::pictureTaken(ofPixels &pixels) {
    card &c = cards[slider.getCurrentPage()];
    pixels.crop(cam.getWidth()-ofGetWidth(), cam.getHeight()-ofGetHeight(), ofGetWidth(), ofGetHeight());
    
    photo.setFromPixels(pixels);
    
    
    offscreen.begin();
    ofSetColor(255);
    photo.draw(ofGetWidth(),ofGetHeight(),-ofGetWidth(),-ofGetHeight());
    c.mask.draw(0, 0);
    
    glReadPixels(0, 0, this->pixels.getWidth(), this->pixels.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, this->pixels.getPixels());
    
    offscreen.end();
    photo.setFromPixels(this->pixels);
    photo.setImageType(OF_IMAGE_COLOR);
    photo.saveImage(ofxiPhoneGetDocumentsDirectory()+"temp.jpg",OF_IMAGE_QUALITY_MEDIUM); //
    //c.buf=ofBufferFromFile(ofxiPhoneGetDocumentsDirectory()+"temp.jpg");
    
    
}

void testApp::volumeButtonPressed(int &button) {
    switch (button) {
        case VOLUME_BUTTON_UP:
            cam.snap();
            break;
        case VOLUME_BUTTON_DOWN:
            photo.clear();
            break;
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void testApp::newResponse(ofxHttpResponse & response){
	//responseStr = ofToString(response.status) + ": " + (string)response.responseBody;
}


