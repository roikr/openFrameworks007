#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(34, 34, 34);
	ofSetVerticalSync(false);
	ofEnableAlphaBlending();
    //ofDisableArbTex();
    //ofDisableNormalizedTexCoords();
    
	camWidth 		= 320;	// try to grab at this size. 
	camHeight 		= 240;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
	
	videoInverted 	= new unsigned char[camWidth*camHeight*3];
	videoTexture.allocate(camWidth,camHeight, GL_RGB);	
    
    
	shader.load("shaders/rgbd.vp", "shaders/rgbd.fp");
    doShader = true;
    
   
    
   
	
}


//--------------------------------------------------------------
void testApp::update(){
	
	
    
   
	
	vidGrabber.grabFrame();
	
	if (vidGrabber.isFrameNew()){
		int totalPixels = camWidth*camHeight*3;
		unsigned char * pixels = vidGrabber.getPixels();
		for (int i = 0; i < totalPixels; i++){
			videoInverted[i] = 255 - pixels[i];
		}
		videoTexture.loadData(videoInverted, camWidth,camHeight, GL_RGB);
	}
    
    

}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetHexColor(0xffffff);
	vidGrabber.draw(20,20);
	videoTexture.draw(20+camWidth,20,camWidth,camHeight);
    
    
    
    ofSetColor(245, 58, 135);
	
	
	if( doShader ){
		shader.begin();
        
        
	}
	
    ofFill();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
	ofRotateX(mouseY);
	ofRotateY(mouseX);
    
    
//    glUniform1i(glGetUniformLocation(shader.getProgram(), "textureUnit0"), videoTexture.getTextureData().textureID);
    
    ofRect(-100, -100, 200, 200);
	
	if( doShader ){
		shader.end();
	}

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	// in fullscreen mode, on a pc at least, the 
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...
	
	if (key == 'v' || key == 'V'){
		vidGrabber.videoSettings();
	}
	
    if( key == 's' ){
		doShader = !doShader;
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