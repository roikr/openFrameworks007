#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(320,240);
	
    colorImg.allocate(320,240);
	
	gauss_foregroundImg.allocate(320,240);
	gauss_backgroundImg.allocate(320,240);
	
	fgd_foregroundImg.allocate(320,240);
	fgd_backgroundImg.allocate(320,240);
	
    CvGaussBGStatModelParams* params = new CvGaussBGStatModelParams;                  
    params->win_size=2;   
    params->n_gauss=5;
    params->bg_threshold=0.7;
    params->std_threshold=3.5;
    params->minArea=15;
    params->weight_init=0.05;
    params->variance_init=30; 
	
    //bgModel = cvCreateGaussianBGModel(colorImg.getCvImage() ,params);
	gauss_bgModel = cvCreateGaussianBGModel(colorImg.getCvImage());
	
	fgd_bgModel = cvCreateFGDStatModel(colorImg.getCvImage());
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);
	
	
	vidGrabber.grabFrame();
	
	
	if (vidGrabber.isFrameNew()){
		
		colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
		
		// Update models
		cvUpdateBGStatModel(colorImg.getCvImage() ,gauss_bgModel);
		gauss_foregroundImg = gauss_bgModel->foreground;
		gauss_backgroundImg = gauss_bgModel->background;
		
		cvUpdateBGStatModel(colorImg.getCvImage() ,fgd_bgModel);
		fgd_foregroundImg = fgd_bgModel->foreground;
		fgd_backgroundImg = fgd_bgModel->background;
	}   
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(0xffffff);
	colorImg.draw(20,20);
	
	gauss_foregroundImg.draw(360,20);
	gauss_backgroundImg.draw(360,280);
	
	fgd_foregroundImg.draw(700,20);
	fgd_backgroundImg.draw(700,280);
	
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	
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