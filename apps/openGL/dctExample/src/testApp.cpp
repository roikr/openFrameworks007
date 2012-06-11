#include "testApp.h"
#include "glUtil.h"

#define GetGLError()									\
{														\
GLenum err = glGetError();							\
while (err != GL_NO_ERROR) {						\
printf("GLError %s set in File:%s Line:%d\n",	\
GetGLErrorString(err),					\
__FILE__,								\
__LINE__);								\
err = glGetError();								\
}													\
}

// Indicies to which we will set vertex array attibutes
// See buildVAO and buildProgram
enum {
	POS_ATTRIB_IDX,
	TEXCOORD_ATTRIB_IDX
};

int loadTexture(ofPixels &pixels,GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLuint textureID;
    
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);

    
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
    
    GetGLError();
    
	return textureID;
}


void testApp::setupShader(ofShader &shader,string filename) {
    shader.checkAndCreateProgram(); 
    glBindAttribLocation(shader.getProgram(),POS_ATTRIB_IDX,"vVertex");
    glBindAttribLocation(shader.getProgram(),TEXCOORD_ATTRIB_IDX,"vTexCoord0");
    shader.load("shaders/VS.vp", filename);
	shader.printActiveUniforms();
}

//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
    
    const GLubyte * strVersion = glGetString(GL_VERSION); 
	printf("version: %s\n",strVersion);
    
    GetGLError();
    
    
    //shader.load("shaders/identity.vp", "shaders/identity.fp");
    
    vertices.push_back(ofVec2f(-1.0,-1.0));
    vertices.push_back(ofVec2f(-1.0,1.0));
    vertices.push_back(ofVec2f(1.0,-1.0));
    vertices.push_back(ofVec2f(1.0,1.0));
    
    // need to flip texture y coords 
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo[POS_ATTRIB_IDX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() * 2 , vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POS_ATTRIB_IDX);
    glVertexAttribPointer(POS_ATTRIB_IDX, 2, GL_FLOAT, GL_FALSE, 0,(const GLvoid *)0);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_ATTRIB_IDX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texCoords.size() * 2 , texCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEXCOORD_ATTRIB_IDX);
    glVertexAttribPointer(TEXCOORD_ATTRIB_IDX, 2, GL_FLOAT, GL_FALSE, 0,(const GLvoid *)0);
   
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    ofImage image;
    image.setUseTexture(false);
    image.loadImage(ofToDataPath("test.png"));
    
    textureID = loadTexture(image.getPixelsRef(), GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    
    setupShader(dct_rows_fprog,"shaders/DCT_rows_pass1_PS.fp");
    setupShader(dct_rows2_fprog,"shaders/DCT_rows_pass2_PS.fp");
    setupShader(dct_unpack_rows_fprog,"shaders/DCT_unpack_rows_PS.fp");
	setupShader(dct_cols_fprog,"shaders/DCT_cols_pass1_PS.fp");
    setupShader(dct_cols2_fprog,"shaders/DCT_cols_pass2_PS.fp");
	setupShader(dct_unpack_cols_fprog,"shaders/DCT_unpack_cols_PS.fp");
	setupShader(idct_cols_fprog,"shaders/IDCT_cols_pass1_PS.fp");
	setupShader(idct_cols2_fprog,"shaders/IDCT_cols_pass2_PS.fp");
    setupShader(idct_unpack_cols_fprog,"shaders/IDCT_unpack_cols_PS.fp");
	setupShader(idct_rows_fprog,"shaders/IDCT_rows_pass1_PS.fp");
	setupShader(idct_rows2_fprog,"shaders/IDCT_rows_pass2_PS.fp");
	setupShader(idct_unpack_rows_fprog,"shaders/IDCT_unpack_rows_PS.fp");
	
   
    
    shader.checkAndCreateProgram(); // need to bind attributes before loading
    
    glBindAttribLocation(shader.getProgram(),POS_ATTRIB_IDX,"vVertex");
    glBindAttribLocation(shader.getProgram(),TEXCOORD_ATTRIB_IDX,"vTexCoord0");
    
    shader.load("shaders/identity.vp", "shaders/identity.fp");
	shader.printActiveUniforms();
       
    fbo[0].setup(image.width, image.height);
    fbo[1].setup(image.width, image.height);
    fbo[2].setup(image.width, image.height);
        
    
    ofBackgroundHex(0x000000,255);
    
    GetGLError();
    
    
    
   
    

    
}



//--------------------------------------------------------------
void testApp::update(){
    
    
    

}

void setUniformTexture(ofShader &shader,string name, int textureTarget,int textureID, int textureLocation) {
    glActiveTexture(GL_TEXTURE0 + textureLocation);
    glEnable(textureTarget);
    glBindTexture(textureTarget, textureID);
    glDisable(textureTarget);
    shader.setUniform1i(name.c_str(), textureLocation);
    glActiveTexture(GL_TEXTURE0);
	
}

void testApp::pass(ofShader &shader,GLuint src1,GLuint src2,ofxFBO &fbo) {
    fbo.begin();
	shader.begin();
	glBindVertexArray(vao);
    setUniformTexture(dct_unpack_rows_fprog,"image", GL_TEXTURE_2D,src1, 0);
    if (src2) {
        setUniformTexture(shader,"image2", GL_TEXTURE_2D,src2, 1);
    }
    
    glUniform1f(glGetUniformLocation(shader.getProgram(), "width"), fbo.width);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()); // GL_LINE_STRIP or GL_TRIANGLE_STRIP
    shader.end();
    fbo.end();
}

//--------------------------------------------------------------
void testApp::draw(){

    // Bind our  FBO and render our scene
    
    pass(dct_rows_fprog,textureID,0,fbo[0]);
    pass(dct_rows2_fprog,textureID,0,fbo[1]);
    pass(dct_unpack_rows_fprog,fbo[0].colorTexture,fbo[1].colorTexture,fbo[2]);
    pass(dct_cols_fprog,fbo[2].colorTexture,0,fbo[0]);
    pass(dct_cols2_fprog,fbo[2].colorTexture,0,fbo[1]);
    pass(dct_unpack_cols_fprog,fbo[0].colorTexture,fbo[1].colorTexture,fbo[2]);
    
    pass(idct_cols_fprog,fbo[2].colorTexture,0,fbo[0]);
    pass(idct_cols2_fprog,fbo[2].colorTexture,0,fbo[1]);
    pass(idct_unpack_cols_fprog,fbo[0].colorTexture,fbo[1].colorTexture,fbo[2]);
    pass(idct_rows_fprog,fbo[2].colorTexture,0,fbo[0]);
    pass(idct_rows2_fprog,fbo[2].colorTexture,0,fbo[1]);
    pass(dct_unpack_rows_fprog,fbo[0].colorTexture,fbo[1].colorTexture,fbo[2]);
    
    shader.begin();
	glBindVertexArray(vao);
	setUniformTexture(shader,"image", GL_TEXTURE_2D,fbo[2].colorTexture, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size()); // GL_LINE_STRIP or GL_TRIANGLE_STRIP
    shader.end();
    
    return;
    
    
    
	shader.begin();
	    
//	glActiveTexture(GL_TEXTURE0);
//	// Bind the texture we rendered-to above (i.e. the reflection texture)
//	glBindTexture(GL_TEXTURE_2D, fbo[2].colorTexture);
    
    // Bind our vertex array object
	glBindVertexArray(vao);
	setUniformTexture(shader,"image", GL_TEXTURE_2D,fbo[2].colorTexture, 0);

    
//#if !ESSENTIAL_GL_PRACTICES_IOS
//	// Generate mipmaps from the rendered-to base level
//	//   Mipmaps reduce shimmering pixels due to better filtering
//	// This call is not accelarated on iOS 4 so do not use
//	//   mipmaps here
//	glGenerateMipmap(GL_TEXTURE_2D);
//#endif
	
	
	
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

