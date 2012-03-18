//
//  ofxStringUtils.h
//  cityViewer
//
//  Created by Roee Kremer on 3/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <string>
using namespace std;

void SetLocale(string locale);
wstring StringToWString(const string& s);
string WStringToString(const wstring& s);