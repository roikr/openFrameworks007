//
//  ofxStringUtils.cpp
//  cityViewer
//
//  Created by Roee Kremer on 3/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxStringUtils.h"
#include <iostream>

/*
 #include "Poco/String.h"
 
 //--------------------------------------------------
 vector <wstring> ofSplitWString(const wstring & source, const string & delimiter, bool ignoreEmpty = false, bool trim = false);
 
 vector <wstring> ofSplitWString(const wstring & source, const string & delimiter, bool ignoreEmpty, bool trim) {
 vector<wstring> result;
 if (delimiter.empty()) {
 result.push_back(source);
 return result;
 }
 wstring::const_iterator substart = source.begin(), subend;
 while (true) {
 subend = search(substart, source.end(), delimiter.begin(), delimiter.end());
 wstring sub(substart, subend);
 if(trim) {
 Poco::trimInPlace(sub);
 }
 if (!ignoreEmpty || !sub.empty()) {
 result.push_back(sub);
 }
 if (subend == source.end()) {
 break;
 }
 substart = subend + delimiter.size();
 }
 return result;
 }
 
 */

void SetLocale(string locale) {
    char* local = setlocale(LC_ALL, locale.c_str());
    cout << "Current LC_CTYPE is " << local << endl;
}


wstring StringToWString(const string& s)
{
    //    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
    //    cout << "Current LC_CTYPE is " << local << endl;
    //    
    wchar_t buffer[100];
    int length = mbstowcs(buffer, s.c_str(), 100);
    
    //    printf( "The number of bytes that comprise the widechar "
    //           "string is %i\n", length );
    
    return wstring(buffer,length);
}


string WStringToString(const wstring& s)
{
    
    //    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
    //    cout << "Current LC_CTYPE is " << local << endl;
    //    
    char buffer[100];
    int length = wcstombs ( buffer, s.c_str(), 100);
    
    //    printf( "The number of bytes that comprise the multibyte "
    //           "string is %i\n", length );
    //    
    return string(buffer,length);
}

