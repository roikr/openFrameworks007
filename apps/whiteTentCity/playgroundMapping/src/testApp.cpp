#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    overlay.load();
	remoteCam.initGrabber(640,480);
	remoteCam.start();
    
    counter=0;
}

//--------------------------------------------------------------
void testApp::update(){
	remoteCam.update();
	if(remoteCam.isFrameNew()){
		//PROCESS DATA
		//remoteCam.getPixels()
		
	}
	
}


//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(20,20,20,255);
	remoteCam.draw(0,0);
	ofDrawBitmapString("CLIENT - FPS: "+ofToString(remoteCam.getFPS(), 1), 10, remoteCam.getCamHeight()+10,false);
	int y=remoteCam.getCamHeight()+40;
	string m="Data Rate: "+ofToString( remoteCam.getLastDataSize(), 0)+"/"+ofToString(640*480*3,0)+" - Rate: "+ofToString(640*480*3, 5);
	ofDrawBitmapString(m, 10, y,false);
	
	

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	switch (key) {
		case 'G':
        case 'g': {
            ofImage colorImage;
            colorImage.setFromPixels(remoteCam.getPixels(),640,480,OF_IMAGE_COLOR);
            colorImage.saveImage("remoteCam_"+ofToString(counter)+".png");
            
            
            
            counter++;
        }
            
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
void testApp::resized(int w, int h){

}

//--------------------------------------------------------------
void testApp::exit(){
	remoteCam.close();
}