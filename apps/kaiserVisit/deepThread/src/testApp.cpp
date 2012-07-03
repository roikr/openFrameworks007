#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	scale = 0.125   ;
	pos = ofVec2f(ofGetWidth()/2,ofGetHeight()/2);
		
	//load();
    deep.setup("IMAGE_1", 5053, 3517, ofRectangle(256,256, 512,256));
    deep.transform(pos, scale);
}



//--------------------------------------------------------------
void testApp::update(){

	deep.update();
	
}

//--------------------------------------------------------------
void testApp::draw(){
	

	
    deep.draw();
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	switch (key) {
		case '+':
		case '-': {
			float newScale = scale;
			
			switch (key) {
				case '+':
					newScale+=0.05;
					break;
				case '-':
					newScale-=0.05;
					break;
					
				default:
					break;
			}
			
            scale = newScale;
			deep.transform(pos, scale);
        
			
		}  break;

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
	ofPoint curPos = ofPoint(x,y);
	pos += curPos-downPos;
	downPos = curPos;
    deep.transform(pos, scale);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	downPos = ofPoint(x,y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	printf("width: %d, height: %d\n",ofGetWidth(),ofGetHeight());
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}