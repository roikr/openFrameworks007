#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	
	shader.load("shaders/tutorial2.vert", "shaders/Metablob.frag");
//	shader.load("shaders/tutorial2.vert", "shaders/Metatunnel.frag");
//	shader.load("shaders/tutorial2.vert", "shaders/Sult.frag");
//	shader.load("shaders/tutorial2.vert", "shaders/Valleyball.frag");
//	shader.load("shaders/tutorial2.vert", "shaders/MengerSponge.frag");
//    shader.load("shaders/tutorial2.vert", "shaders/Slisesix.frag");
//    shader.load("shaders/tutorial2.vert", "shaders/Quaternion.frag");
//    shader.load("shaders/tutorial2.vert", "shaders/Lunaquatic.frag");
//    shader.load("shaders/tutorial2.vert", "shaders/Mandelbulb.frag");
 //   shader.load("shaders/tutorial2.vert", "shaders/Leizex.frag");

    // shader.load("shaders/tutorial2.vert", "shaders/tutorial2.frag");
    
    
    
    
	shader.printActiveUniforms();
		
	
	velocity = 1;

	const GLubyte * strVersion = glGetString(GL_VERSION); 
	printf("version: %s\n",strVersion);
    
    
    int i; /* Simple iterator */
        
    /* We're going to create a simple diamond made from lines */
    const GLfloat rect[4][2] = {
        {  -1.0,  -1.0  }, /* Top point */
        {  -1.0,  1.0  }, /* Right point */
        {  1.0, -1.0  }, /* Bottom point */
        {  1.0,  1.0  } }; /* Left point */
    
   
    /* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &vao);
    
    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(vao);
    
    /* Allocate and assign two Vertex Buffer Objects to our handle */
    glGenBuffers(2, vbo);
    
    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    
    /* Copy the vertex data from diamond to our buffer */
    /* 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values */
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), rect, GL_STATIC_DRAW);
    
    /* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    /* Enable attribute index 0 as being used */
    glEnableVertexAttribArray(0);
    
    
//    const GLfloat colors[4][3] = {
//        {  1.0,  0.0,  0.0  }, /* Red */
//        {  0.0,  1.0,  0.0  }, /* Green */
//        {  0.0,  0.0,  1.0  }, /* Blue */
//        {  1.0,  1.0,  1.0  } }; /* White */
    
//    /* Bind our second VBO as being the active buffer and storing vertex attributes (colors) */
//    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//    
//    /* Copy the color data from colors to our buffer */
//    /* 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values */
//    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
//    
//    /* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    
//    /* Enable attribute index 1 as being used */
//    glEnableVertexAttribArray(1);
	
}


//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    
//    glClearColor ( 0.0, 1.0, 0.0, 1.0 );
//    glClear ( GL_COLOR_BUFFER_BIT );
    
    /* Make our background black */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

	float w = ofGetWidth();
	float h = ofGetHeight();
    
//    printf("w: %f, h: %f\n",w,h);
	
	shader.begin();
    
	//we want to pass in some varrying values to animate our type / color 
	shader.setUniform1f("time", ofGetElapsedTimef() * velocity );
	shader.setUniform2f("resolution", w,h);
    
    
//	glBegin(GL_QUADS);
//	glVertex3f(0, 0, 0);
//	glVertex3f(w/2, 0, 0);
//	glVertex3f(w/2, h/2, 0);
//	glVertex3f(0,h/2, 0);
//	glEnd();
//    
    //glRectd(0.5,0.5,0.25,0.25);
    
    
            
    /* Invoke glDrawArrays telling that our data is a line loop and we want to draw 2-4 vertexes */
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
   
    
    
   
  
	
	shader.end();
	

}

void testApp::exit() {
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	switch (key) {
		case '+':
			velocity+=0.1;
			break;
		case '-':
			velocity-=0.1;
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

