#include "testApp.h"


int loadTexture(string filename,GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLuint textureID;
    
    ofImage image;
    image.setUseTexture(false);
    image.loadImage(ofToDataPath(filename));
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);

    
    ofPixels pixels =image.getPixelsRef();
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, ofGetGlInternalFormat(pixels), pixels.getWidth(), pixels.getHeight(), 0,
				 ofGetGlInternalFormat(pixels), GL_UNSIGNED_BYTE, pixels.getPixels());
	
    
    
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
    
    
    textureID = loadTexture(ofToDataPath("texture.jpeg"), GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
        
    shader.checkAndCreateProgram();
    
    
    //shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    vertices.push_back(ofVec2f(-0.5,-0.5));
    vertices.push_back(ofVec2f(-0.5,0.5));
    vertices.push_back(ofVec2f(0.5,-0.5));
    vertices.push_back(ofVec2f(0.5,0.5));
    
    texCoords.push_back(ofVec2f(0.0,0.0));
    texCoords.push_back(ofVec2f(0.0,1.0));
    texCoords.push_back(ofVec2f(1.0,0.0));
    texCoords.push_back(ofVec2f(1.0,1.0));
    
    
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
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    shader.begin();
    
     glUniform1i(glGetUniformLocation(shader.getProgram(), "textureUnit0"), 0);  
   
    //shader.setUniformTexture("uSampler", texture.getTextureReference(), 0); 
    shader.setUniform4f("vColor", 0.5f,1.0f,1.0f,1.0f);
    
    shader.end();
   
}



//--------------------------------------------------------------
void testApp::update(){
    
    
    

}

//--------------------------------------------------------------
void testApp::draw(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    
    
   
    
	shader.begin();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()); // GL_LINE_STRIP or GL_TRIANGLE_STRIP
    
    shader.end();
   
}

void testApp::exit() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2,vbo);
    glDeleteTextures(1, &textureID);
    //texture.clear();
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

