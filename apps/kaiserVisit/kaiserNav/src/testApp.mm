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
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
    ofMatrix4x4 mat;
    
    composition = doc.getSymbolItem("COMPOSITION_1")->createInstance("composition", mat);
//    layout.getChild("pimp")->bVisible = false;
    background = composition.getLayer("background");
    outlines = composition.getLayer("outlines");
    
    captionMat = mat;
    captionMat.preMult(composition.getChild("CAPTION_1")->mat);
    caption = doc.getSymbolItem("CAPTION_1")->createInstance("caption", captionMat);
    

    //    layout.getChild("pimp")->bVisible = false;
    
	
	ofEnableAlphaBlending();
	
	ofSetCircleResolution(32);
	
	cam.setZoom(0.125f);
//    cam.setZoom(1.0f);
	cam.setMinZoom(0.1f);
	cam.setMaxZoom(2.0f);
	cam.setScreenSize( ofGetWidth(), ofGetHeight() );
	cam.setViewportConstrain( ofVec3f(-limitX, -limitY), ofVec3f(limitX, limitY)); //limit browseable area, in world units
	
//    deep.setup("IMAGE_1", "png",5053, 3517, ofRectangle(100,100, ofGetWidth()-200,ofGetHeight()-200));
    width = 5053;
    height = 3517;
    deep.setup("IMAGE_1", "png",width, height, ofRectangle(0,0, ofGetWidth(),ofGetHeight()));
    deep.transform( camOffset(), cam.zoom);
    
    deep.start();

	floating.setup(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()), 150, 150);
}


void testApp::update() {
    cam.update();
    
    if (cam.getIsAnimating()) {
        deep.transform( camOffset(), cam.zoom);
        floating.setAnchor(cam.worldToScreen(captionMat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height))); 
        caption.mat.makeTranslationMatrix(floating.getPos());

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
    
        
    
        composition.drawLayer(outlines);
        
        
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
	
    ofPushStyle();
    ofSetColor(0);
    ofSetLineWidth(2);
    ofVec2f vec = floating.getAnchor()-floating.getPos();
    vec = vec.normalized()*(vec.length()-200*cam.zoom);
    ofLine(floating.getPos(), floating.getPos()+vec);
    ofPopStyle();
    caption.draw();
    
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
    
    floating.setAnchor(cam.worldToScreen(captionMat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height))); 
    caption.mat.makeTranslationMatrix(floating.getPos());
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
