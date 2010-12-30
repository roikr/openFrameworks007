/*
 *  ofMainExt.h
 *  iPhoneSantaExample
 *
 *  Created by Roee Kremer on 12/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofTypes.h"

#include <string>
using namespace std;

string ofToDocumentsPath(string path);
string ofToResourcesPath(string path);

void ofxiPhoneGLViewPort();

float distance(ofPoint pnt);

