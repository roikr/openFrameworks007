#include "testApp.h"

#define MARGIN 50

//--------------------------------------------------------------
void testApp::setup(){

	fbo.allocate(ofGetWidth(), ofGetHeight(),GL_RGBA);
	
	
	fbo.begin();
	ofClear(255, 255, 255);
	ofFill();
	
	ofSetHexColor(0x000000);
	ofRect(MARGIN, MARGIN , ofGetWidth()-2*MARGIN, ofGetHeight()-2*MARGIN);
	fbo.end();
	
	fbo.readToPixels(pixels);
	
	
	
    imagen = cvCreateImage( cvSize(ofGetWidth(), ofGetHeight()), IPL_DEPTH_8U, 1 );
	
	//fbo.setup();
	bStroke = false;
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);
}

//--------------------------------------------------------------
void testApp::draw(){

	
	if (!bStroke && !positions.empty()) {
		fbo.begin();
		ofSetHexColor(0xffffff);
		for (vector<ofPoint>::iterator iter=positions.begin(); iter!=positions.end(); iter++) {
			vector<ofPoint>::iterator niter=iter+1;
			if (niter!=positions.end()) {
				ofLine(iter->x, iter->y, niter->x, niter->y);
			}
			
		}
		fbo.end();
		positions.clear();
		fbo.readToPixels(pixels);
	}
	
	ofSetHexColor(0xffffff);
	fbo.draw(0, 0);	
	
	for (vector<ofPoint>::iterator iter=positions.begin(); iter!=positions.end(); iter++) {
		vector<ofPoint>::iterator niter=iter+1;
		if (niter!=positions.end()) {
			ofLine(iter->x, iter->y, niter->x, niter->y);
		}
		
	}

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if (bStroke) {
		if (!positions.empty()) {
			if (positions.size() == 1) {
				if (pixels[(y*fbo.getWidth()+x)*pixels.getBytesPerPixel()]) {
					positions.clear();
				} 
				positions.push_back(ofPoint(x,y));
			} else {
				positions.push_back(ofPoint(x,y));
				if (pixels[(y*fbo.getWidth()+x)*pixels.getBytesPerPixel()]) {
					bStroke = false;
				} 
			}
		}
	}
	
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	positions.clear();
	
	if (pixels[(y*fbo.getWidth()+x)*pixels.getBytesPerPixel()]) {
		positions.push_back(ofPoint(x,y));
		bStroke = true;
	} else {
		
		
		for (int i=0; i<fbo.getHeight();i++) {
			for (int j=0; j<fbo.getWidth(); j++) {
				imagen->imageData[i*(int)fbo.getWidth()+j] = pixels[(i*fbo.getWidth()+j)*pixels.getBytesPerPixel()];
			}
		}
		
		cvFloodFill(imagen, cvPoint(x, y), cvScalarAll(255)  );
		
		for (int i=0; i<fbo.getHeight();i++) {
			for (int j=0; j<fbo.getWidth(); j++) {
				pixels[(i*(int)fbo.getWidth()+j)*pixels.getBytesPerPixel()] = imagen->imageData[i*(int)fbo.getWidth()+j];
			}
		}
		
		fbo.getTextureReference().loadData(pixels);
	}

	
	//printf("pixel: %i",);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if	(bStroke) {
		if (positions.size() > 1 && pixels[(y*fbo.getWidth()+x)*pixels.getBytesPerPixel()]) {
			positions.push_back(ofPoint(x,y));
		} else {
			positions.clear();	
		}
	}
	
	bStroke = false;
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
