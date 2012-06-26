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
    
    material.setUseTexture(false);
    material.loadImage("Marble.jpg");
    
    cam.setup();
    
    depthID = createTexture( GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    colorID = createTexture( GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    materialID = createTexture( GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,depthID ); //vidGrabber.getTextureReference().getTextureData().textureID
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,colorID ); //vidGrabber.getTextureReference().getTextureData().textureID
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,materialID );
    
    shader.checkAndCreateProgram();
    
    
    //shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    int numRows = 30;
    int numColumns = 40; 
    
    float dx = 1.0/numColumns;
    float dy = 1.0/numRows;
    
    for (int y=0;y<numRows;y++) {
        for (int x=0;x<numColumns;x++) {
            
//            vertices.push_back(ofVec2f(-0.5,-0.5));
//            vertices.push_back(ofVec2f(-0.5,0.5));
//            vertices.push_back(ofVec2f(0.5,-0.5));
//            vertices.push_back(ofVec2f(0.5,0.5));
            
            float fx = (float)x*dx;
            float fy = (float)y*dy;
            vertices.push_back(ofVec2f(fx,fy)-ofVec2f(0.5,0.5));
            vertices.push_back(ofVec2f(fx,fy+dy)-ofVec2f(0.5,0.5));
            vertices.push_back(ofVec2f(fx+dx,fy)-ofVec2f(0.5,0.5));
            vertices.push_back(ofVec2f(fx,fy+dy)-ofVec2f(0.5,0.5));
            vertices.push_back(ofVec2f(fx+dx,fy)-ofVec2f(0.5,0.5));
            vertices.push_back(ofVec2f(fx+dx,fy+dy)-ofVec2f(0.5,0.5));
            
//            vertices.push_back(ofVec2f(fx,fy)-ofVec2f(1.0,1.0));
//            vertices.push_back(ofVec2f(fx,fy+dy)-ofVec2f(1.0,1.0));
//            vertices.push_back(ofVec2f(fx+dx,fy)-ofVec2f(1.0,1.0));
//            vertices.push_back(ofVec2f(fx,fy+dy)-ofVec2f(1.0,1.0));
//            vertices.push_back(ofVec2f(fx+dx,fy)-ofVec2f(1.0,1.0));
//            vertices.push_back(ofVec2f(fx+dx,fy+dy)-ofVec2f(1.0,1.0));
            
            
//            texCoords.push_back(ofVec2f(0.0,1.0));
//            texCoords.push_back(ofVec2f(0.0,0.0));
//            texCoords.push_back(ofVec2f(1.0,1.0));
//            texCoords.push_back(ofVec2f(0.0,0.0));
//            texCoords.push_back(ofVec2f(1.0,1.0));
//            texCoords.push_back(ofVec2f(1.0,0.0));
            
            
            
            float ty = 1-fy;
            texCoords.push_back(ofVec2f(fx,ty));
            texCoords.push_back(ofVec2f(fx,ty-dy));
            texCoords.push_back(ofVec2f(fx+dx,ty));
            texCoords.push_back(ofVec2f(fx,ty-dy));
            texCoords.push_back(ofVec2f(fx+dx,ty));
            texCoords.push_back(ofVec2f(fx+dx,ty-dy));

        }
    }
    
    
    
    
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
    
//    shader.setGeometryInputType(GL_TRIANGLE_STRIP);
//	shader.setGeometryOutputType(GL_TRIANGLE_STRIP);
//	shader.setGeometryOutputCount(4);
    shader.load("shaders/identity.vp", "shaders/identity.fp");//,"shaders/geom.glsl");
    
    
	shader.printActiveUniforms();
    
    
   
    
    glEnable(GL_DEPTH_TEST);
    
   
    
    shader.begin();
    
    glUniform1i(glGetUniformLocation(shader.getProgram(), "depthTexture"), 0);  
    glUniform1i(glGetUniformLocation(shader.getProgram(), "colorTexture"), 1);
    glUniform1i(glGetUniformLocation(shader.getProgram(), "materialTexture"), 2);
    
    //shader.setUniformTexture("uSampler", texture.getTextureReference(), 0); 
    //shader.setUniform4f("vColor", 1.0f,1.0f,1.0f,1.0f);
    
        
    //float colorMat[9] = {0.393,0.769,0.189,0.349,0.686,0.168,0.272,0.534,0.131};
    float colorMat[9] = {0.393,0.349,0.272,0.769,0.686,0.534,0.189,0.168,0.131};
    
    glUniformMatrix3fv(glGetUniformLocation(shader.getProgram(), "colorTransform"),1,false,colorMat );  
    
    
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixels.getWidth(), pixels.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.getPixels());
    
    glActiveTexture(GL_TEXTURE2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, material.getWidth(), material.getHeight(), 0,GL_RGB, GL_UNSIGNED_BYTE, material.getPixels());
}

//--------------------------------------------------------------
void testApp::draw(){
//    image.draw(0, 0);
//    depth.draw(image.getWidth(),0);
    
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    
    ofPushMatrix();
    
    
//    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
//	ofRotateX(mouseY);
//	ofRotateY(mouseX);
    

    
    ofMatrix4x4 projMat,mvMat;
//    glGetFloatv(GL_PROJECTION_MATRIX,projMat.getPtr());
//    glGetFloatv(GL_MODELVIEW_MATRIX, mvMat.getPtr());
    
    mvMat.rotate(mouseY,1.0,0,0);
    mvMat.rotate(mouseX, 0, 1.0, 0);
    
	shader.begin();
    
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "projMatrix"),1,false,projMat.getPtr() );  
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "modelViewMatrix"),1,false,mvMat.getPtr() );  
    
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // GL_LINE_STRIP or GL_TRIANGLE_STRIP
    
    shader.end();
    
    ofPopMatrix();
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