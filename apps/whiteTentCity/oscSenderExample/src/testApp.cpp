#include "testApp.h"

#define HOST "192.168.10.143" //"localhost"
#define PORT 12345

string WStringToString(const wstring& s)
{
    
    
    char buffer[100];
    int length = wcstombs ( buffer, s.c_str(), 100);
    
    //    printf( "The number of bytes that comprise the multibyte "
    //           "string is %i\n", length );
    
    return string(buffer,length);
    
    
}

//--------------------------------------------------------------
void testApp::setup(){

//    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
//    cout << "Current LC_CTYPE is " << local << endl;
    printf("Locale is: %s\n",setlocale(LC_ALL, "he_IL.UTF-8"));
    
    
	ofBackground( 40, 100, 40 );

	// open an outgoing connection to HOST:PORT
	sender.setup( HOST, PORT );
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	// display instructions
	string buf;
	buf = "sending osc messages to " + string( HOST ) + ":" + ofToString( PORT );
	ofDrawBitmapString( buf, 10, 20 );
}



//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if ( key =='a' || key == 'A' ){
		ofxOscMessage m;
		m.setAddress( "/sendMessage" );
		m.addIntArg( 4 );
		
		m.addStringArg(WStringToString(L"יתרבח קדצ שרוד םעה"));
       
		sender.sendMessage( m );
	}
    
    if ( key =='b' || key == 'B' ){
		ofxOscMessage m;
		m.setAddress( "/sendMessage" );
		m.addIntArg( 4 );
		
        m.addStringArg(WStringToString(L"ןוטלשל טייטש"));
        
		sender.sendMessage( m );
	}
    
    if ( key =='c' || key == 'C' ){
		ofxOscMessage m;
		m.setAddress( "/sendMessage" );
		m.addIntArg( 4 );
		
        m.addStringArg(WStringToString(L"why like this"));
        
		sender.sendMessage( m );
	}
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

