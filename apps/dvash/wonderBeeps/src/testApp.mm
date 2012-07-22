#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	    
    keyboard = new ofxiPhoneKeyboard(80,60,160,40);
	keyboard->setVisible(false);
   	keyboard->setBgColor(75, 75, 75, 255);
	keyboard->setFontColor(150,150,150, 255);
	keyboard->setFontSize(32);

	    
    font.loadFont("segoescb.ttf", 24);
	
	int bufferSize = 256;
    float size = ofGetWidth()/4;
    
    for (int i=0; i<16;i++) {
        button b;
        string filename = "beep"+ofToString(i)+".caf";
        cout << filename << ": " << b.audio.load(ofToDataPath(filename), bufferSize) << endl;
        b.rect = ofRectangle(i%4*size,floor(i/4)*size+(ofGetHeight()-ofGetWidth())/2,size,size);
        b.text = ofToString(i+1);
        buttons.push_back(b);
    }
    
    ofxXmlSettings xml;
    if (xml.loadFile(ofxiPhoneGetDocumentsDirectory()+"buttons.xml")) {
        xml.pushTag("buttons");
        for (int i=0;i<xml.getNumTags("button");i++) {
            string text = xml.getAttribute("button", "text", "",i);
            buttons[i].text = text;
            
        }
        xml.popTag();
    }
    
    
    
	
    
	ofSoundStreamSetup(2, 0, this, 44100, bufferSize, 2);
	
    ofBackground(75);
    ofSetColor(150);

    ofEnableAlphaBlending();
    background.setUseTexture(false);
    background.loadImage("Default@2x.png");
    background.setImageType(OF_IMAGE_GRAYSCALE);
    background.setUseTexture(true);
    background.reloadTexture();
    
    bEditMode = false;
    bButtonDown = false;
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    
    
    
    if(keyboard->isKeyboardShowing()) {
        buttons[keyNum].text = keyboard->getLabelText();
    } else {
        if (bEditMode) {
            bEditMode = false;
            keyboard->setVisible(false);
            
            ofxXmlSettings xml;
            xml.addTag("buttons");
            xml.pushTag("buttons");
            
            for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
                int i = distance(buttons.begin(), iter);
                xml.addTag("button");
                xml.addAttribute("button", "text", iter->text,i);
            }
            xml.popTag();
                
            xml.saveFile(ofxiPhoneGetDocumentsDirectory()+"buttons.xml");
        }
    }
    
    bool bPlaying = false;
    for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
        
        if (iter->audio.getNumPlaying()) {
            bPlaying = true;
            break;
        }
    }
    
    if (!bPlaying && !queue.empty()) {
        buttons[queue.back()].audio.play();
        queue.pop_back();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255, 255, 255, 25);
    background.draw(0, 0);
	for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
        
        ofSetColor(150);
        
        if (iter->audio.getNumPlaying() || (bButtonDown && distance(buttons.begin(), iter) == keyNum)) {
            ofFill();
            ofRect(iter->rect);
            ofSetColor(75);
           
        } else {
            ofNoFill();
            ofRect(iter->rect);
        }
        
        float width = font.stringWidth(iter->text);
        float height = font.stringHeight(iter->text);
        
        ofVec2f pos = iter->rect.getCenter()+0.5*ofVec2f(-width,height);
        font.drawString(iter->text, pos.x, pos.y);
        
        
    }
}

//--------------------------------------------------------------
void testApp::exit(){

}

void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	memset(output, 0, sizeof(float)*bufferSize*nChannels);
    for (vector<button>::iterator iter = buttons.begin(); iter!=buttons.end(); iter++) {
        
        if (iter->audio.getNumPlaying()) {
            iter->audio.mixChannel(output, 0, nChannels);
            iter->audio.mixChannel(output, 1, nChannels);
            iter->audio.postProcess();
        }
    }
	
	
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    if (touch.id == 0) {
        
        
        vector<button>::iterator iter;
        for (iter = buttons.begin(); iter!=buttons.end(); iter++) {
            
            if (iter->rect.inside(ofVec2f(touch.x,touch.y))) {
                break;
            }
        }
        
        if (iter!=buttons.end()){
            downTime = ofGetElapsedTimeMillis();
            keyNum = distance(buttons.begin(), iter);
            bButtonDown = true;
        }
    }
    
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    if (touch.id == 0 && bButtonDown) {
        bButtonDown = false;
        if (ofGetElapsedTimeMillis()-downTime>500) {
            
            bEditMode = true;
           
            if(!keyboard->isKeyboardShowing()){
                keyboard->setVisible(true);
                keyboard->setText(buttons[keyNum].text);
                keyboard->openKeyboard();
                
            } 
            
        } else {
            queue.push_front(keyNum);
            
        }
    }
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

void testApp::touchCancelled(ofTouchEventArgs &touch) {
    
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

