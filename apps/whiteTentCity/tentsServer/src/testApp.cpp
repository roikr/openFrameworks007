#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    server = ofxHTTPServer::getServer(); // get the instance of the server
	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir("upload");		 // folder to save uploaded files
	server->setCallbackExtension("asp");	 // extension of urls that aren't files but will generate a post or get event
	ofAddListener(server->getEvent,this,&testApp::getRequest);
//	ofAddListener(server->postEvent,this,&testApp::postRequest);
	server->start(8888);
}

void testApp::getRequest(ofxHTTPServerResponse & response){
	
    response.response="<html> <head> oF http server </head> <body> " + response.url + " <body> </html>";     
   
    cout << response.url << endl;
    for (map<string,string>::iterator iter=response.requestFields.begin();iter!=response.requestFields.end();iter++) {
        cout << iter->first << ": " << iter->second << endl;
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