#include "testApp.h"

#define SAMPLE_XML_PATH "SamplesConfig.xml"


int createTexture(GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLuint textureID;
    
    
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	
    
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
	return textureID;
}

//--------------------------------------------------------------
void testApp::setup(){
       
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    const GLubyte * strVersion = glGetString(GL_VERSION); 
	printf("version: %s\n",strVersion);
    
    cam.setup();
    
    depthID = createTexture( GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    colorID = createTexture( GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,depthID ); //vidGrabber.getTextureReference().getTextureData().textureID
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,colorID ); //vidGrabber.getTextureReference().getTextureData().textureID
    

    
    
    shader.checkAndCreateProgram();
    
    
    //shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    vertices.push_back(ofVec2f(-0.5,-0.5));
    vertices.push_back(ofVec2f(-0.5,0.5));
    vertices.push_back(ofVec2f(0.5,-0.5));
    vertices.push_back(ofVec2f(0.5,0.5));
    
    texCoords.push_back(ofVec2f(0.0,1.0));
    texCoords.push_back(ofVec2f(0.0,0.0));
    texCoords.push_back(ofVec2f(1.0,1.0));
    texCoords.push_back(ofVec2f(1.0,0.0));
    
    
    /* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &vao);
    
    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(vao);
    
    /* Allocate and assign two Vertex Buffer Objects to our handle */
    glGenBuffers(2, vbo);
    
    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() * 2 , vertices.data(), GL_STATIC_DRAW);
    glBindAttribLocation(shader.getProgram(),0,"vVertex");
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0,(const GLvoid *)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texCoords.size() * 2 , texCoords.data(), GL_STATIC_DRAW);
    glBindAttribLocation(shader.getProgram(),1,"vTexCoord0");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,(const GLvoid *)0);
    glEnableVertexAttribArray(1);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    
	shader.printActiveUniforms();
    
    
   
    
    glEnable(GL_DEPTH_TEST);
    
   
    
    shader.begin();
    
    glUniform1i(glGetUniformLocation(shader.getProgram(), "depthTexture"), 0);  
    
    glUniform1i(glGetUniformLocation(shader.getProgram(), "colorTexture"), 1);  
    
    //shader.setUniformTexture("uSampler", texture.getTextureReference(), 0); 
    shader.setUniform4f("vColor", 1.0f,1.0f,1.0f,1.0f);
    
    shader.end();
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    cam.update();
    
   // shader.begin();
    
    ofPixels pixels = cam.getDepthPixels();
    

    
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixels.getWidth(), pixels.getHeight(), 0,
				 GL_RED, GL_UNSIGNED_BYTE, pixels.getPixels());
    
    
    
    pixels = cam.getImagePixels();
    
    glActiveTexture(GL_TEXTURE1);    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixels.getWidth(), pixels.getHeight(), 0,
				 GL_RGB, GL_UNSIGNED_BYTE, pixels.getPixels());
    
   
}

//--------------------------------------------------------------
void testApp::draw(){
//    image.draw(0, 0);
//    depth.draw(image.getWidth(),0);
    
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    
    
    
    
	shader.begin();
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()); // GL_LINE_STRIP or GL_TRIANGLE_STRIP
    
    shader.end();
}

void testApp::exit() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2,vbo);
    glDeleteTextures(1, &depthID);
    glDeleteTextures(2, &colorID);
    //texture.clear();
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