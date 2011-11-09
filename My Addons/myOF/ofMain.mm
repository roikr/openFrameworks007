/*
 *  ofMain.cpp
 *  YesPlastic
 *
 *  Created by Roee Kremer on 1/19/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include <OpenGLES/ES1/gl.h>

#include "sys/time.h" // for gettimeofday

#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision

#include <dirent.h> 
#include <sys/stat.h> 

#include "ofxiPhoneExtras.h" // for orientation
#include "ofMainExt.h" // for ofToResourcesPath()

static bool enableDataPath = true; 

#define 	OF_FILLED				0x01
#define 	OF_OUTLINE				0x02
#define 	OF_WINDOW 				0
#define 	OF_FULLSCREEN 			1
#define 	OF_GAME_MODE			2

#define 	OF_RECTMODE_CORNER				0
#define 	OF_RECTMODE_CENTER				1


void ofRegisterTouchEvents(ofBaseApp *app) {
	
}


static float	drawMode			= OF_FILLED;
bool 			bBakgroundAuto		= true;
int 			cornerMode			= OF_RECTMODE_CORNER;
static float linePoints[4];
static float rectPoints[8];


//--------------------------------------------------

void ofEnableDataPath(){
	enableDataPath = true;
}

//--------------------------------------------------
void ofDisableDataPath(){
	enableDataPath = false;
}

//string ofToDataPath(string path, bool makeAbsolute){	
//	if (enableDataPath) {
//		return ofToDocumentsPath("data/"+path);
//	} else {
//		return path;
//	}
//
//	
//}

static string dataPathRoot = "data/";

//--------------------------------------------------
void ofSetDataPathRoot(string newRoot){
	string newPath = "";
	dataPathRoot = newPath+newRoot;
}


//--------------------------------------------------
string ofToDataPath(string path, bool makeAbsolute){
	if( enableDataPath ){
		
		//check if absolute path has been passed or if data path has already been applied
		//do we want to check for C: D: etc ?? like  substr(1, 2) == ':' ??
		if( path.length()==0 || (path.substr(0,1) != "/" &&  path.substr(1,1) != ":" &&  path.substr(0,dataPathRoot.length()) != dataPathRoot)){
			path = dataPathRoot+path;
		}
		
		if(makeAbsolute && (path.length()==0 || path.substr(0,1) != "/")){
#ifdef TARGET_OF_IPHONE
			//do we need iphone specific code here?
#endif
		}
		
	}
	return path;
}



//--------------------------------------------------
void ofLog(int logLevel, string message){
	
	/*
	if(logLevel == OF_LOG_VERBOSE){
		printf("OF_VERBOSE: ");
	}
	else if(logLevel == OF_LOG_NOTICE){
		printf("OF_NOTICE: ");
	}
	else if(logLevel == OF_LOG_WARNING){
		printf("OF_WARNING: ");
	}
	else if(logLevel == OF_LOG_ERROR){
		printf("OF_ERROR: ");
	}
	else if(logLevel == OF_LOG_FATAL_ERROR){
		printf("OF_FATAL_ERROR: ");
	}
	 */
	printf("%s\n",message.c_str());	
	
}

//--------------------------------------------------
void ofLog(int logLevel, const char* format, ...){
	//thanks stefan!
	//http://www.ozzu.com/cpp-tutorials/tutorial-writing-custom-printf-wrapper-function-t89166.html
	va_list args;
	va_start( args, format );
	if(logLevel == OF_LOG_VERBOSE){
		printf("OF_VERBOSE: ");
	}
	else if(logLevel == OF_LOG_NOTICE){
		printf("OF_NOTICE: ");
	}
	else if(logLevel == OF_LOG_WARNING){
		printf("OF_WARNING: ");
	}
	else if(logLevel == OF_LOG_ERROR){
		printf("OF_ERROR: ");
	}
	else if(logLevel == OF_LOG_FATAL_ERROR){
		printf("OF_FATAL_ERROR: ");
	}
	vprintf( format, args );
	printf("\n");
	va_end( args );
	
		
	
}

void ofSetLogLevel(int logLevel) {
}

//--------------------------------------------------
string ofToString(double value, int precision){
	stringstream sstr;
	sstr << fixed << setprecision(precision) << value;
	return sstr.str();
}

//--------------------------------------------------
string ofToString(int value){
	stringstream sstr;
	sstr << value;
	return sstr.str();
}

//our openGL wrappers
//----------------------------------------------------------

void ofBackground(int r, int g, int b) {
	
	float 			bgColor[4]			= {0,0,0,0};
	bgColor[0] = (float)r / (float)255.0f;
	bgColor[1] = (float)g / (float)255.0f;
	bgColor[2] = (float)b / (float)255.0f;
	bgColor[3] = 1.0f;
	// if we are in not-auto mode, then clear with a bg call...
	
	glClearColor(bgColor[0],bgColor[1],bgColor[2], bgColor[3]);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
}

void ofPushMatrix(){
	glPushMatrix();
}

//----------------------------------------------------------
void ofPopMatrix(){
	glPopMatrix();
}

//----------------------------------------------------------
void ofTranslate(float x, float y, float z){
	glTranslatef(x, y, z);
}

//----------------------------------------------------------
void ofScale(float xAmnt, float yAmnt, float zAmnt){
	glScalef(xAmnt, yAmnt, zAmnt);
}

//----------------------------------------------------------
void ofRotate(float degrees, float vecX, float vecY, float vecZ){
	glRotatef(degrees, vecX, vecY, vecZ);
}

void ofRotate(float degrees){
	glRotatef(degrees, 0, 0, 1);
}


void ofSetColor(int _r, int _g, int _b){
	float r = (float)_r / 255.0f; r = MAX(0,MIN(r,1.0f));
	float g = (float)_g / 255.0f; g = MAX(0,MIN(g,1.0f));
	float b = (float)_b / 255.0f; b = MAX(0,MIN(b,1.0f));
	
	
	
	glColor4f(r,g,b,1);
}


//----------------------------------------------------------
void ofSetColor(int _r, int _g, int _b, int _a){
	float r = (float)_r / 255.0f; r = MAX(0,MIN(r,1.0f));
	float g = (float)_g / 255.0f; g = MAX(0,MIN(g,1.0f));
	float b = (float)_b / 255.0f; b = MAX(0,MIN(b,1.0f));
	float a = (float)_a / 255.0f; a = MAX(0,MIN(a,1.0f));
	
		
	glColor4f(r,g,b,a);
}


//----------------------------------------------------------
void ofLine(float x1,float y1,float x2,float y2){
	
	
	
	
	linePoints[0] = x1;
	linePoints[1] = y1;
	linePoints[2] = x2;
	linePoints[3] = y2;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &linePoints[0]);
	glDrawArrays(GL_LINES, 0, 2);
	
	
	
}

//----------------------------------------------------------
void ofNoFill(){
	drawMode = OF_OUTLINE;
	
}

//----------------------------------------------------------
void ofFill(){
	drawMode = OF_FILLED;
	
}

//----------------------------------------------------------
void ofRect(float x,float y,float w,float h){
	
	
	
	if (cornerMode == OF_RECTMODE_CORNER){
		rectPoints[0] = x;
		rectPoints[1] = y;
		
		rectPoints[2] = x+w;
		rectPoints[3] = y;
		
		rectPoints[4] = x+w;
		rectPoints[5] = y+h;
		
		rectPoints[6] = x;
		rectPoints[7] = y+h;
	}else{
		rectPoints[0] = x-w/2;
		rectPoints[1] = y-h/2;
		
		rectPoints[2] = x+w/2;
		rectPoints[3] = y-h/2;
		
		rectPoints[4] = x+w/2;
		rectPoints[5] = y+h/2;
		
		rectPoints[6] = x-w/2;
		rectPoints[7] = y+h/2;
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &rectPoints[0]);
	glDrawArrays((drawMode == OF_FILLED) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
	
	
	
}


//----------------------------------------------------------
void ofSetColor(int hexColor){
	int r = (hexColor >> 16) & 0xff;
	int g = (hexColor >> 8) & 0xff;
	int b = (hexColor >> 0) & 0xff;
	ofSetColor(r,g,b);
}

//----------------------------------------------------------
void ofEnableAlphaBlending(){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

//----------------------------------------------------------
void ofDisableAlphaBlending(){
	glDisable(GL_BLEND);
	
}



float ofClamp(float value, float min, float max) {
	return value < min ? min : value > max ? max : value;
}

unsigned long ofGetSystemTime( ) {
	struct timeval now;
	gettimeofday( &now, NULL );
	return now.tv_usec/1000 + now.tv_sec*1000;
}

static unsigned long startTime = ofGetSystemTime();   //  better at the first frame ?? (currently, there is some delay from static init, to running.

//--------------------------------------
int ofGetElapsedTimeMillis(){
	return (int)(ofGetSystemTime() - startTime);
}


//ofTrueTypeFont::ofTrueTypeFont() {}
// ofTrueTypeFont::~ofTrueTypeFont() {}
//void 		ofTrueTypeFont::loadFont(string filename, int fontsize) {}
//void 		ofTrueTypeFont::drawString(string s, float x, float y) {}
//void 		ofTrueTypeFont::setLineHeight(float height) {}




int ofGetWidth() {

    return orientation == OFXIPHONE_ORIENTATION_PORTRAIT || orientation == OFXIPHONE_ORIENTATION_UPSIDEDOWN ? narrowDimension : wideDimension;
}

int ofGetHeight() {
    return orientation == OFXIPHONE_ORIENTATION_PORTRAIT || orientation == OFXIPHONE_ORIENTATION_UPSIDEDOWN ? wideDimension : narrowDimension;
}

float 		ofGetFrameRate() {
	return 30.0f;
}

void 		ofSetFrameRate(int targetRate) {}

//--------------------------------------------------
vector<string> ofSplitString(const string& str, const string& delimiter = " "){
    vector<string> elements;
	// Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiter, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiter, lastPos);
	
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
    	elements.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiter, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiter, lastPos);
    }
    return elements;
}



//--------------------------------------------------
void ofSeedRandom() {
	
	// good info here:
	// http://stackoverflow.com/questions/322938/recommended-way-to-initialize-srand
	
#ifdef TARGET_WIN32
	srand(GetTickCount());
#else
	// use XOR'd second, microsecond precision AND pid as seed
	struct timeval tv;
	gettimeofday(&tv, 0);
	long int n = (tv.tv_sec ^ tv.tv_usec) ^ getpid();
	srand(n);
#endif
}

//--------------------------------------------------
void ofSeedRandom(int val) {
	srand((long) val);
}

//--------------------------------------------------
float ofRandom(float x, float y) {
	
	float high = 0;
	float low = 0;
	float randNum = 0;
	// if there is no range, return the value
	if (x == y) return x; 			// float == ?, wise? epsilon?
	high = MAX(x,y);
	low = MIN(x,y);
	randNum = low + ((high-low) * rand()/(RAND_MAX + 1.0));
	return randNum;
}

//--------------------------------------------------
float ofRandomf() {
	float randNum = 0;
	randNum = (rand()/(RAND_MAX + 1.0)) * 2.0 - 1.0;
	return randNum;
}

//--------------------------------------------------
float ofRandomuf() {
	float randNum = 0;
	randNum = rand()/(RAND_MAX + 1.0);
	return randNum;
}



