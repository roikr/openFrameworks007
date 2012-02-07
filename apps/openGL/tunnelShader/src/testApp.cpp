#include "testApp.h"


void testApp::generateGeometry() {
	
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);  // OF_PRIMITIVE_LINE_STRIP or OF_PRIMITIVE_TRIANGLES
	
	float radius = 7;
	float currentRadius = radius;
	float segments = 24;
	float spacing = 2;
	float numRings = 18;
	float index=0;
    
	
	for (float ring=0; ring<numRings; ring++)
	{
		for(float segment=0; segment<segments; segment++)
		{
			float degrees = (360/segments) * segment;
			float radians = (PI/180) * degrees;
			float x = cos(radians) * currentRadius;
			float y = sin(radians) * currentRadius;
			float z = ring * -spacing;
			
			ofVec3f vec = ofVec3f(x,y,z);
			mesh.addVertex(vec);
            
			
            
			ofVec2f texCoord;
			
			if(segment < (segments-1)/ 2) {
				texCoord = ofVec2f((1.0/(segments))*segment*2, (1.0/4)*ring);
			}
			else {
				texCoord = ofVec2f(2.0-((1.0/(segments))*segment*2), (1.0/4)*ring);
			}
			
			mesh.addTexCoord(texCoord);
           
            //printf("vertex: %f, %f, %f, texCoord: %f, %f\n",vec.x,vec.y,vec.z,texCoord.x,texCoord.y);
           
			
			
			float color = 1.0-((1.0/(numRings-1))*ring);
			ofFloatColor fcolor = ofFloatColor(color,color,color,1.0);
			mesh.addColor(fcolor);
			
            
			
			if(ring<numRings-1) {
				if(segment < segments-1) {
					mesh.addTriangle((ofIndexType)index, (ofIndexType)(index + segments + 1), (ofIndexType)(index + segments));
					mesh.addTriangle((ofIndexType)index, (ofIndexType)(index+1), (ofIndexType)(index + segments + 1));
				} else {
					mesh.addTriangle((ofIndexType)index, (ofIndexType)(index + 1), (ofIndexType)(index + segments));
					mesh.addTriangle((ofIndexType)index, (ofIndexType)(index - segments + 1), (ofIndexType)(index + 1));
				}
			}
            
            
			
			index++;
		}
		currentRadius -= radius/numRings;
	}
}

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
    
   // textureID = loadTexture(ofToDataPath("texture.jpeg"), GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    textureID = loadTexture(ofToDataPath("texture.jpeg"), GL_NEAREST, GL_NEAREST, GL_REPEAT);
    
    shader.checkAndCreateProgram();
    
    
    
    //shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    
    generateGeometry();
    
    
    
    /* Allocate and assign a Vertex Array Object to our handle */
    glGenVertexArrays(1, &vao);
    
    /* Bind our Vertex Array Object as the current used object */
    glBindVertexArray(vao);
    
    /* Allocate and assign two Vertex Buffer Objects to our handle */
    glGenBuffers(4, vbo);
    
    /* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh.getNumVertices() * 3 , mesh.getVerticesPointer(), GL_STATIC_DRAW);
    glBindAttribLocation(shader.getProgram(),0,"aVertexPosition");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,(const GLvoid *)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh.getNumColors() * 4 , mesh.getColorsPointer(), GL_STATIC_DRAW);
    glBindAttribLocation(shader.getProgram(),1,"aVertexColor");
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,(const GLvoid *)0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh.getNumTexCoords() * 2 , mesh.getTexCoordsPointer(), GL_STATIC_DRAW);
    glBindAttribLocation(shader.getProgram(),2,"aTextureCoord");
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,(const GLvoid *)0);
    glEnableVertexAttribArray(2);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ofIndexType) * mesh.getNumIndices() , mesh.getIndexPointer(), GL_STATIC_DRAW);

   
    pMatrix.makePerspectiveMatrix(45, (float)ofGetWidth()/(float)ofGetHeight(), 0.1, 100.0);
    //pMatrix.makeOrtho2DMatrix(-10, 10, -10, 10);
    mvMatrix.makeIdentityMatrix();
    mvMatrix.translate(0.0, 0.0, -8.0);
    
    shader.load("shaders/tunnel.vert", "shaders/tunnel.frag");
    
    
	shader.printActiveUniforms();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
   
    
    
    shader.begin();
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(),"uPMatrix"),1, GL_FALSE,pMatrix.getPtr());
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(),"uMVMatrix"),1, GL_FALSE,mvMatrix.getPtr());
        
    glUniform1i(glGetUniformLocation(shader.getProgram(), "uSampler"), 0);  
    
    
//    shader.setUniformTexture("uSampler", texture.getTextureReference(), 0); 
//    shader.setUniform4f("vColor", 1.0f,1.0f,1.0f,0.0f);
//    glActiveTexture(GL_TEXTURE0);
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, texture.getTextureReference().getTextureData().textureID);
//    GLint iTextureUniform = glGetUniformLocation(shader.getProgram(),"uSampler");
//    glUniform1i(iTextureUniform, 0);
    
    
    shader.end();
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
   
}



//--------------------------------------------------------------
void testApp::update(){
    
    
    

}

//--------------------------------------------------------------
void testApp::draw(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
	shader.begin();
    
    
    shader.setUniform1f("fTime", ofGetElapsedTimef());
    
    glDrawElements(ofGetGLPrimitiveMode(mesh.getMode()), mesh.getNumIndices(),GL_UNSIGNED_INT , (const GLvoid *)0);
    
    shader.end();
   
}

void testApp::exit() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(4,vbo);
    glDeleteTextures(1, &textureID);
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

