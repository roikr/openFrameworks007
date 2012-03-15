#include "testApp.h"


wstring StringToWString(const string& s);
string WStringToString(const wstring& s);

wstring StringToWString(const string& s)
{
    wstring temp(s.length(),L' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp; 
}


string WStringToString(const wstring& s)
{
    string temp(s.length(), ' ');
    copy(s.begin(), s.end(), temp.begin());
    return temp; 
}

//--------------------------------------------------------------
void testApp::setup(){
    
   
        
    
    char  buffer[40];
    int length;
    
    
   //char* local = setlocale(LC_CTYPE, NULL);
    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
    cout << "Current LC_CTYPE is " << local << endl;
    
//    wchar_t wch=wstr[0];
//    length = wctomb( buffer, wch );
//    printf( "The number of bytes that comprise the multibyte "
//           "character is %i\n", length );
    
    wstring wstr=L"םולש";
        
    length = wcstombs ( buffer, wstr.c_str(), 40 );
    printf( "The number of bytes that comprise the multibyte "
           "string is %i\n", length );
    
    string str(buffer,length);

    cout << str << endl;
    
    wchar_t wbuf[40];
    length = mbstowcs(wbuf, str.c_str(), 40);
    printf( "The number of bytes that comprise the widechar "
           "string is %i\n", length );
    
    wstring word(wbuf,length);
    //length = mbstowcs(buffer, <#const char *#>, <#size_t#>)

    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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