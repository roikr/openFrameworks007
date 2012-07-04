#include "testApp.h"

int limitX = 6000;	//these define where the camera can pan to
int limitY = 4000;

ofVec2f testApp::camOffset( ){
	
    //return (p-0.5*ofVec2f(limitX,limitY)+cam.offset)*cam.zoom+0.5f*ofVec2f(ofGetWidth(),ofGetHeight());
    return cam.offset*cam.zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
}

void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	ofxiPhoneSetOrientation( OF_ORIENTATION_90_LEFT );	
	// initialize the accelerometer
	ofxAccelerometer.setup();	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	ofEnableAlphaBlending();
	
	ofSetCircleResolution(32);
	
	cam.setZoom(0.125f);
//    cam.setZoom(1.0f);
	cam.setMinZoom(0.1f);
	cam.setMaxZoom(2.0f);
	cam.setScreenSize( ofGetWidth(), ofGetHeight() );
	cam.setViewportConstrain( ofVec3f(-limitX, -limitY), ofVec3f(limitX, limitY)); //limit browseable area, in world units
	
//    deep.setup("IMAGE_1", "png",5053, 3517, ofRectangle(100,100, ofGetWidth()-200,ofGetHeight()-200));
    deep.setup("IMAGE_1", "png",5053, 3517, ofRectangle(0,0, ofGetWidth(),ofGetHeight()));
    deep.transform( camOffset(), cam.zoom);
    
    deep.start();

	
}


void testApp::update() {
    cam.update();
    
    if (cam.getIsAnimating()) {
        deep.transform( camOffset(), cam.zoom);
    }
    
	deep.update();
}


void testApp::draw() {
//    deep.begin();
//    deep.draw();
//    deep.end();
		
	cam.apply(); //put all our drawing under the ofxPanZoom effect
        ofPushMatrix();
        deep.draw();
        ofPopMatrix();
    
    
		//draw grid
		
	
		//draw space constrains		
		int s = 25;
		glColor4f(1, 0, 0, 1);
		ofRect(-limitX , -limitY , 2 * limitX, s);
		ofRect(limitX - s , -limitY , s, 2 * limitY);
		ofRect(-limitX , limitY - s , s, -2 * limitY);	
		ofRect(limitX , limitY - s, -2 * limitX, s);		
		glColor4f(1, 1, 1, 1);
	
	cam.reset();	//back to normal ofSetupScreen() projection
	
	cam.drawDebug(); //see info on ofxPanZoom status
	
	glColor4f(1,1,1,1);
	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );	
}

void testApp::exit() {
//    deep.stop();
}

void testApp::touchDown(ofTouchEventArgs &touch){

	cam.touchDown(touch); //fw event to cam
	
	ofVec3f p =  cam.screenToWorld( ofVec3f( touch.x, touch.y) );	//convert touch (in screen units) to world units
	
}


void testApp::touchMoved(ofTouchEventArgs &touch){
	cam.touchMoved(touch); //fw event to cam
    deep.transform(camOffset(), cam.zoom);
}


void testApp::touchUp(ofTouchEventArgs &touch){
	cam.touchUp(touch);	//fw event to cam
}


void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	cam.touchDoubleTap(touch); //fw event to cam
	cam.setZoom(1.0f);	//reset zoom
	cam.lookAt( ofVec3f() ); //reset position
}


void testApp::touchCancelled(ofTouchEventArgs& args){

}


void testApp::deviceOrientationChanged(int newOrientation){
	//not sure whiy this is inverted in horizontal modes, disabling for now
	//ofxiPhoneSetOrientation( (ofOrientation)newOrientation);
	//cam.setScreenSize(ofGetWidth(), ofGetHeight());
};
