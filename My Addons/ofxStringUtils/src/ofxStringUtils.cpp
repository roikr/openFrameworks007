//
//  ofxStringUtils.cpp
//  cityViewer
//
//  Created by Roee Kremer on 3/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxStringUtils.h"
#include <iostream>
#include "ofConstants.h"

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

    printf ("Locale is: %s\n", setlocale(LC_ALL,locale.c_str()));
//    char* local = setlocale(LC_ALL, locale.c_str());
//    cout << "Current LC_CTYPE is " << local << endl;

    
}


wstring StringToWString(const string& str)
{
#ifdef TARGET_WIN32
    // Convert an ASCII string to a Unicode String
    std::wstring wstrTo;
    wchar_t *wszTo = new wchar_t[str.length() + 1];
    wszTo[str.size()] = L'\0';
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wszTo, (int)str.length());
    wstrTo = wszTo;
    delete[] wszTo;
    return wstrTo;
#else
    //    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
    //    cout << "Current LC_CTYPE is " << local << endl;
    //    
    wchar_t buffer[100];
    int length = mbstowcs(buffer, str.c_str(), 100);
    
    //    printf( "The number of bytes that comprise the widechar "
    //           "string is %i\n", length );
    
    return wstring(buffer,length);
#endif
}


string WStringToString(const wstring& wstr)
{
#ifdef TARGET_WIN32
     // Convert a Unicode string to an ASCII string
    std::string strTo;
    char *szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
#else    
    //    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
    //    cout << "Current LC_CTYPE is " << local << endl;
    //    
    char buffer[100];
    int length = wcstombs ( buffer, wstr.c_str(), 100);
    
    //    printf( "The number of bytes that comprise the multibyte "
    //           "string is %i\n", length );
    //    
    return string(buffer,length);
#endif
}

