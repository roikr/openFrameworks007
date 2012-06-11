#include "testApp.h"
#include <iostream>
#include <GLUT/GLUT.h>
#include "ofxProCamToolkit.h"


//--------------------------------------------------------------
void testApp::setup()
{
	ofBackground(0, 0, 0, 1);
	ofSetFrameRate(30);
	ofSetVerticalSync(false);
	ofEnableSmoothing();
	ofEnableAlphaBlending();
	//ofEnableNormalizedTexCoords();
	//ofHideCursor();
	
	glNewList(1, GL_COMPILE);
	glutSolidSphere(0.5, 40, 40);
	glEndList();
	
	int boidNum = 250;
	//target = ofVec3f(0, 0, 0);
	
	for (int i = 0; i < boidNum; i++)
	{
		Star v(ofRandom(100)-50, ofRandom(100)-50, ofRandom(100)-50);
		v.maxForce = 0.3f;
		v.inSightDist = 30.0f;
		boids.push_back(v);
	}
	
	cam.setDistance(300);
	
	GLfloat color[] = { 1.0, 1.0, 1.0 };
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    
    
}

//--------------------------------------------------------------
void testApp::update()
{
	for(vector<Star>::iterator iter = boids.begin();iter!=boids.end();iter++) {
    
    
        if (iter->bStarMode) {
            iter->position.z+=3;
        } else {
            iter->flock(boids);
            iter->update();
            iter->wrap(300, 300, 300);
        }
        
        iter->brightness = ofMap(iter->position.z, -50, 300, 0.1, 1.0,true);
            
        cam.begin();
        ofVec3f pos = ofWorldToScreen(iter->position);
        cam.end();
        
        pos.z = 0;
        if ( (ofVec3f(ofGetWidth()/2,ofGetHeight()/2,0.0f)-pos).length()>ofGetHeight()/2) {
            if (iter->bStarMode) {
                iter->bVisible = true;
                iter->position = ofVec3f(ofRandom(100)-50, 0, 0);
                iter->position.rotate(ofRandom(360), ofVec3f(0,0,1));
            } else {
                iter->bVisible = false;
            }
        } else {
            iter->bVisible = true;
        }
    } 
}

//--------------------------------------------------------------
void testApp::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	cam.begin();
    
    
    
    for(vector<Star>::iterator iter = boids.begin();iter!=boids.end();iter++) {
        if (iter->bVisible) {
       
            glPushMatrix();
            glTranslatef(iter->position.x, iter->position.y, iter->position.z);
            
            //GLfloat color[] = { 0.8, 0.2, 0.2, 1.0 };
            GLfloat color[] = { 1.0 * iter->brightness, 1.0 * iter->brightness, 1.0 * iter->brightness, 1.0 };
            
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
            glCallList(1);
            glPopMatrix();
        }
        

    }
	
		
	cam.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'S':
        case 's':
            for(vector<Star>::iterator iter = boids.begin();iter!=boids.end();iter++) 
                iter->bStarMode = !iter->bStarMode;
            break;
            
        default:
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
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}