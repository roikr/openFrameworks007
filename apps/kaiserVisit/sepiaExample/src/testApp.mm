#include "testApp.h"

ofColor transformColor(ofColor &src) {
    
    //    static ofMatrix4x4 colorMat(0.393,0.349,0.272,0.0,0.769,0.686,0.534,0.0,0.189,0.168,0.131,0.0,0.0,0.0,0.0,1.0);
    static ofMatrix4x4 colorMat(0.393,0.769,0.189,0.0,0.349,0.686,0.168,0.0,0.272,0.534,0.131,0.0,0.0,0.0,0.0,1.0);
    ofVec4f dst = colorMat.postMult(ofVec4f(src.r,src.g,src.b,255.0)/255.0)*255;
    
    return ofColor(dst.x,dst.y,dst.z).clamp();
}


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
	
	ofBackground(127,127,127);
    ofxRegisterStillCameraNotification(this);
    cam.preview();
    
    ofColor src(255,255,255);
    ofColor dst(transformColor(src));
    
    cout << (int)dst.r << "\t" << (int)dst.g << "\t" << (int)dst.b << endl;

}

//--------------------------------------------------------------
void testApp::update(){
    cam.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
    
    if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
        float width = ofGetWidth(); 
        float height = width/cam.getWidth()*cam.getHeight();
        cam.draw(ofRectangle(0,0,width,height), ofRectangle(0,0,1,1));
    } 
    
    if (image.bAllocated()) {
        image.draw(10, 10, image.getWidth()/4, image.getHeight()/4);
    }
	
    glColor4f(1,1,1,1);
	ofDrawBitmapString("of fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 50 );
	ofDrawBitmapString("video fps: " + ofToString( cam.getFrameRate(), 1 ),  10, ofGetHeight() - 30 );
    ofDrawBitmapString(ofToString( cam.getWidth())+" "+ofToString(cam.getHeight()),  10, ofGetHeight() - 10 );
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    if (cam.getIsPlaying()) {
        cam.snap();
    }
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    
    
   
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


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

void testApp::pictureTaken(ofImage &image) {
    
    this->image.setFromPixels(image.getPixelsRef());
    this->image.update();
    
    UInt8 *data = this->image.getPixels(); // unsigned char
    
    int width = this->image.getWidth();
    int height = this->image.getHeight();
    NSInteger myDataLength = width * height * 3;
    
    for (int i = 0; i < myDataLength; i+=3)
    {
        UInt8 r_pixel = data[i];
        UInt8 g_pixel = data[i+1];
        UInt8 b_pixel = data[i+2];
        
        int outputRed = (r_pixel * .393) + (g_pixel *.769) + (b_pixel * .189);
        int outputGreen = (r_pixel * .349) + (g_pixel *.686) + (b_pixel * .168);
        int outputBlue = (r_pixel * .272) + (g_pixel *.534) + (b_pixel * .131);
        
        if(outputRed>255)outputRed=255;
        if(outputGreen>255)outputGreen=255;
        if(outputBlue>255)outputBlue=255;
        
        
        data[i] = outputRed;
        data[i+1] = outputGreen;
        data[i+2] = outputBlue;
    }
    
    this->image = image;
}
