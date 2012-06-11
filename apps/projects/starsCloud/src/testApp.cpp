#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    star s;
    s.pos = ofVec3f(0,0,0);
    stars.assign(1000, s);
    for(vector<star>::iterator iter=stars.begin();iter!=stars.end();iter++) {
        iter->pos=ofVec3f(ofRandom(-100, 100),ofRandom(-100, 100),ofRandom(-100, 100));
        iter->color.setHsb(ofRandom(255), 50, ofRandom(150,255));
    }
    
    cam.setDistance(100);
    
    ofDisableArbTex();
    texture.loadImage("EarthMap_2500x1250.jpg");
    
    ofBackground(0);
//    
    sphere=NULL;
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluQuadricOrientation( sphere, GLU_OUTSIDE);
    
    
    //Making a display list
    mysphereID = glGenLists(1);
    glNewList(mysphereID, GL_COMPILE);
       
    
    gluSphere(sphere, 1.0, 20, 20);
    glEndList();
    gluDeleteQuadric(sphere);
    
//    glNewList(1, GL_COMPILE);
//	glutSolidSphere(0.5, 40, 40);
//	glEndList();
    
    GLfloat color[] = { 1.0, 1.0, 1.0 };
	
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
//	glLightfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
//    glEnable(GL_DEPTH_TEST);
//    
//    
//    
//   glEnable(GL_COLOR_MATERIAL);
//    glEnable(GL_LIGHTING);
//    
//    glColorMaterial(GL_FRONT, GL_AMBIENT);
//    glColor3f(0.75, 0.75, 0.75);
//    glColorMaterial(GL_FRONT, GL_EMISSION);
//    glColor3f(0.0, 0.0, 0.0);
//    glColorMaterial(GL_FRONT, GL_SPECULAR);
//    glColor3f(0.75, 0.75, 0.75);
//    glColorMaterial(GL_FRONT, GL_DIFFUSE);
//    glColor3f(0.75, 0.75, 0.75);
    
 

}

//--------------------------------------------------------------
void testApp::update(){
    texture.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255);
    cam.begin();
   
   
    for(vector<star>::iterator iter=stars.begin();iter!=stars.end();iter++) {
        ofPushMatrix();
        glTranslatef(iter->pos.x, iter->pos.y, iter->pos.z);
        GLfloat color[] = { iter->color.r/255.0f,iter->color.g/255.0f, iter->color.b/255.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
         
         
//        gluSphere(sphere, 1.0, 20, 20);
//        texture.bind();
       glCallList(mysphereID);
        texture.unbind();
        glPopMatrix();
    }
    
    cam.end();
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