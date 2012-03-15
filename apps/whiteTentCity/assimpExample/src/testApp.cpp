#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);

    // we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();

        
    if(model.loadModel("tent_low.obj",true)){
        
        mesh1 = model.getMesh(0);
    	
    }
    
    if(model.loadModel("tent.obj",true)){
        
        mesh2 = model.getMesh(0);
    	
    }

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	glEnable(GL_DEPTH_TEST);

    //some model / light stuff
    glShadeModel(GL_SMOOTH);
 //   light.enable();
    
    ofEnableSeparateSpecularLight();
//    ofSetGlobalAmbientColor(ofColor(30,30,30));

	
}

//--------------------------------------------------------------
void testApp::update(){
    //model.setRotation(1, ofGetElapsedTimef() * 60, 0, 1, 0);

	


}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(50, 50, 50, 0);
    ofSetColor(255, 255, 255, 255);
    
    cam.begin();
    
    ofPushMatrix();
    
//    model.drawFaces();
    

    
    glRotatef(90, 1.0f, 0.0f, 0.0f);
//    glEnable(GL_NORMALIZE);
//    glScalef(100, 100, 100);
    
    
    mesh1.drawWireframe();
    glScalef(100, 100, 100);
    mesh2.drawWireframe();
    
    ofPopMatrix();
    
    cam.end();
/*
    ofPushMatrix();
		ofTranslate(model.getPosition().x+100, model.getPosition().y, 0);
		ofRotate(-mouseX, 0, 1, 0);
		ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);

		model.drawFaces();

    ofPopMatrix();


    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glEnable(GL_NORMALIZE);
    //glEnable(GL_CULL_FACE);
    ofPushMatrix();

		ofTranslate(model.getPosition().x-300, model.getPosition().y, 0);
		ofRotate(-mouseX, 0, 1, 0);
		ofTranslate(-model.getPosition().x, -model.getPosition().y, 0);

		ofTranslate(position);
		ofRotate(180, 0, 0, 1);
		ofTranslate(-sceneCenter.x, -sceneCenter.y, sceneCenter.z);


		ofScale(normScale , normScale, normScale);


		ofScale(scale.x,scale.y,scale.z);

		tex.bind();
		material.begin();
		mesh.drawWireframe();
		material.end();
		tex.unbind();

	ofPopMatrix();

	glPopAttrib();
*/

    ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate(), 2), 10, 15);
    
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

