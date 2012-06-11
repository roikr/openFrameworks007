#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofBackground(34, 34, 34);
	ofSetVerticalSync(false);
	ofEnableAlphaBlending();
		
	shader.load("shaders/noise.vert", "shaders/noise.frag");
	
	doShader = false;
	
    clouds.setup(0.40,5);
}

//--------------------------------------------------------------
void testApp::update(){
    clouds.update();

}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(225);
	ofDrawBitmapString("'s' toggles shader", 10, 20);
    ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()),10,40);
	//ofSetColor(245, 58, 135);
	ofFill();
	
	if( doShader ){
		shader.begin();
			//we want to pass in some varrying values to animate our type / color 
			shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.1 );
			shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.18 );
			
			//we also pass in the mouse position 
			//we have to transform the coords to what the shader is expecting which is 0,0 in the center and y axis flipped. 
			shader.setUniform2f("mouse", mouseX - ofGetWidth()/2, ofGetHeight()/2-mouseY );

	}
	
		//finally draw our text
    clouds.draw();
    
	
	if( doShader ){
		shader.end();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
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

