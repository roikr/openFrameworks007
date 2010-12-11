/*
 *  ofMainExt.h
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"

#include <string>
using namespace std;

#define		OF_PORTRAIT 0
#define		OF_LANDSCAPE 1


string ofToDocumentsPath(string path);
void ofSetOrientation(int orientation);
string ofToResourcesPath(string path);