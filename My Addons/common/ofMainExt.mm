/*
 *  ofMainExt.cpp
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMainExt.h"
#include "ofxiPhoneExtras.h"
#include "ofMain.h"
#include <OpenGLES/ES1/gl.h>


string ofToDocumentsPath(string path){
	
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
	string documentsPathRoot = [documentsDirectory UTF8String];
	path = documentsPathRoot+"/"+path;
	
	return path;
}

#ifdef MY_OF

string ofToResourcesPath(string path){
	
	//----- DAMIAN
	// set data path root for ofToDataPath()
	// path on iPhone will be ~/Applications/{application GUID}/openFrameworks.app/data
	// get the resource path for the bundle (ie '~/Applications/{application GUID}/openFrameworks.app')
	NSString *bundle_path_ns = [[NSBundle mainBundle] resourcePath];
	// convert to UTF8 STL string
	string dataPathRoot = [bundle_path_ns UTF8String];
	// append data
	dataPathRoot.append( "/data/" ); // ZACH
	//dataPathRoot.append( "/" ); // ZACH
	//printf("setting data path root to '%s'\n", dataPathRoot.c_str() );
	
	
	//check if absolute path has been passed or if data path has already been applied
	//do we want to check for C: D: etc ?? like  substr(1, 2) == ':' ??
	if( path.substr(0,1) != "/" && path.substr(0,dataPathRoot.length()) != dataPathRoot){
		path = dataPathRoot+path;
	}	
	
	return path;
}

void ofxiPhoneGLViewPort() {	
	glViewport( 0, 0, ofGetWidth(), ofGetHeight() );
}


#else // official of

string ofToResourcesPath(string path){
	
	//----- DAMIAN
	// set data path root for ofToDataPath()
	// path on iPhone will be ~/Applications/{application GUID}/openFrameworks.app/data
	// get the resource path for the bundle (ie '~/Applications/{application GUID}/openFrameworks.app')
	NSString *bundle_path_ns = [[NSBundle mainBundle] resourcePath];
	// convert to UTF8 STL string
	string dataPathRoot = [bundle_path_ns UTF8String];
	// append data
	//dataPathRoot.append( "/data/" ); // ZACH
	
	
	dataPathRoot.append( "/" ); // ROIKR
	//printf("setting data path root to '%s'\n", dataPathRoot.c_str() );
	
	
	//check if absolute path has been passed or if datØa path has already been applied
	//do we want to check for C: D: etc ?? like  substr(1, 2) == ':' ??
	if( path.substr(0,1) != "/" && path.substr(0,dataPathRoot.length()) != dataPathRoot){
		path = dataPathRoot+path;
	}	
	
	return path;
}

void ofxiPhoneGLViewPort() {
	if(ofxiPhoneGetOrientation() == OFXIPHONE_ORIENTATION_PORTRAIT || ofxiPhoneGetOrientation() == OFXIPHONE_ORIENTATION_UPSIDEDOWN)
		glViewport( 0, 0, ofGetWidth(), ofGetHeight() );
	else
		glViewport( 0, 0, ofGetHeight(), ofGetWidth() );
}


#endif


float distance(ofPoint pnt) {
	ofPoint norm = pnt*pnt;
	return sqrt(norm.x+norm.y+norm.z);
}
