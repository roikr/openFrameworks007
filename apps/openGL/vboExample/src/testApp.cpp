#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    const GLubyte * strVersion = glGetString(GL_VERSION); 
	printf("version: %s\n",strVersion);
	
    
    shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    
	shader.printActiveUniforms();
    
    /* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &vao);
    
    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(vao);
    

    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh.addVertex(ofVec3f(-0.5,-0.5,0.0));
    mesh.addVertex(ofVec3f(-0.5,0.5,0.0));
    mesh.addVertex(ofVec3f(0.5,-0.5,0.0));
    mesh.addVertex(ofVec3f(0.5,0.5,0.0));
    vbo.setMesh(mesh, GL_STATIC_DRAW);

    
    vbo.bind();
    /* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    /* Enable attribute index 0 as being used */
    glEnableVertexAttribArray(0);
    vbo.unbind();
    
    
	
    
    
    	
}


//--------------------------------------------------------------
void testApp::update(){
    
    
    

}

//--------------------------------------------------------------
void testApp::draw(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
	shader.begin();
    shader.setUniform4f("vColor", 1.0f,1.0f,0.0f,0.0f);

    mesh.draw();
	shader.end();
}

void testApp::exit() {
    glDeleteVertexArrays(1, &vao);
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
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

