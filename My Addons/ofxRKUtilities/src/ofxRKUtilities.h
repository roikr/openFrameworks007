/*
 *  ofxRKUtilities.h
 *  atlasCreatorFromImages
 *
 *  Created by Roee Kremer on 3/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <vector>
using namespace std;

vector<string> ofListFolders(string directory);
bool ofIsFileExists( string filename );