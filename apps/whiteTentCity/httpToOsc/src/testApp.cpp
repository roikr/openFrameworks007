#include "testApp.h"
#include "ofxXmlSettings.h"
#include <algorithm>

//--------------------------------------------------------------
void testApp::setup(){
    
//    char* local = setlocale(LC_ALL, "he_IL.UTF-8");
//    cout << "Current LC_CTYPE is " << local << endl;
    
    ofxXmlSettings xml;
    xml.loadFile("server.xml");
    xml.pushTag("server");
    
    sender.setup( xml.getAttribute("osc", "host", "localhost"), xml.getAttribute("osc", "port", 12345) );
    xml.popTag();

    server = ofxHTTPServer::getServer(); // get the instance of the server
	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir("upload");		 // folder to save uploaded files
	server->setCallbackExtension("asp");	 // extension of urls that aren't files but will generate a post or get event
	ofAddListener(server->getEvent,this,&testApp::getRequest);
//	ofAddListener(server->postEvent,this,&testApp::postRequest);
	server->start(8888);
}


void testApp::getRequest(ofxHTTPServerResponse & response){
	
    
    cout << response.url << endl;
    for (map<string,string>::iterator iter=response.requestFields.begin();iter!=response.requestFields.end();iter++) {
        cout << iter->first << ": " << iter->second << endl;
    }
    
    if (response.url == "/sendMessage.asp") {
        ofxOscMessage m;
		m.setAddress( "/sendMessage" );
		m.addIntArg( ofToInt(response.requestFields["tent"]));
		string message = response.requestFields["message"];
        
        cout << message << endl;
        
        response.response="<html lang='he'> <head> <meta charset='utf-8' /> </head> <body> " + message + " <body> </html>";
        
        cout << message << endl;
        
		m.addStringArg(message);
        
		sender.sendMessage( m );
        
        //response.response="<html> <head> oF http server </head> <body> " + response.url + " <body> </html>";
        
        
    }
    
    if (response.url == "/form.asp") {
        
        
        response.response="<html lang='he'> <head> <meta charset='utf-8' /> </head> <body dir='rtl'> <form  name='example' method='get' action='sendMessage.asp'>מספר אוהל:<input type='text' name='tent'><br/>הודעה:<input type='text' name='message'><br/> <input type='submit' name='submit1' value='שלח'> </form> <body> </html>";
        
        
        //response.response="<html> <head> oF http server </head> <body> " + response.url + " <body> </html>";
        
        
    }
    
    
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