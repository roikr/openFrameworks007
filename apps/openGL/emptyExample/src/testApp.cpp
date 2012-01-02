#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    const GLubyte * strVersion = glGetString(GL_VERSION); 
	printf("version: %s\n",strVersion);
	
    
    shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    
	shader.printActiveUniforms();
    
	
	    /* We're going to create a simple diamond made from lines */
    const GLfloat rect[4][2] = {
        {  -0.5,  -0.5  }, /* Top point */
        {  -0.5,  0.5  }, /* Right point */
        {  0.5, -0.5  }, /* Bottom point */
        {  0.5,  0.5  } }; /* Left point */
    
    
    /* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &vao);
    
    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(vao);
    
    /* Allocate and assign two Vertex Buffer Objects to our handle */
    glGenBuffers(1, &vbo);
    
    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    /* Copy the vertex data from diamond to our buffer */
    /* 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values */
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), rect, GL_STATIC_DRAW);
    
    /* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    /* Enable attribute index 0 as being used */
    glEnableVertexAttribArray(0);
    
    
	
    
    
    	
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

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
	shader.end();
}

void testApp::exit() {
    glDeleteBuffers(1, &vbo);
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

