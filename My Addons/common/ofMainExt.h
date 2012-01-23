/*
 *  ofMainExt.h
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofTypes.h"

#include <string>
using namespace std;

//string ofToDocumentsPath(string path); // deprecated use: string ofxiPhoneGetDocumentsDirectory() at ofxiPhoneExtras
//string ofToResourcesPath(string path);
bool ofxCopyFile(string src,string dest);
bool ofxDeleteFile(string dest);

void ofxiPhoneGLViewPort();

float distance(ofPoint pnt);

//void setiPhoneDataPath(); // deprecated use: ofSetDataPathRoot(ofxNSStringToString([[NSBundle mainBundle] resourcePath])+'/'); or whatever

ofColor ofxHexToColor(int hex);
ofColor ofxHexStringToColor(string color); 
string ofxIntToString(int val,int pad);