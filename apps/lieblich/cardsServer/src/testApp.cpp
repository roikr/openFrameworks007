#include "testApp.h"

#define DELETE_AFTER 60 // minutes
#define NEW_IMAGE_DURATION 20000
#define IMAGE_DURATION 10000 // milis
//--------------------------------------------------------------
void testApp::setup(){
//    ofHideCursor();
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	ofBackground(100);

    ofEnableAlphaBlending();

    upload_dir = "upload";
	server = ofxHTTPServer::getServer(); // get the instance of the server
//	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir(upload_dir);		 // folder to save uploaded files
	server->setCallbackExtension("of");	 // extension of urls that aren't files but will generate a post or get event
	server->setListener(*this);
	server->start(8888);

    posted_filename="";


}



//--------------------------------------------------------------
void testApp::update(){

    if (posted_filename!="") {
        image.loadImage(upload_dir+"/"+posted_filename);
        posted_filename="";
    }

    




}

//--------------------------------------------------------------
void testApp::draw(){
    if (image.isAllocated()) {
        image.draw(0,0);
    }

}




void testApp::getRequest(ofxHTTPServerResponse & response){
    //	if(response.url=="/showScreen.of"){
    //		response.response="<html> <head> <title>oF http server</title> \
    //				<script> \
    //				function beginrefresh(){ \
    //					setTimeout(\"window.location.reload();\",30); \
    //				}\
    //		window.onload=beginrefresh; \
    //		</script>\
    //				</head> <body> <img src=\"screen.jpg\"/> </body> </html>";
    //
    //		imageSaved  = false;
    //	}
}

void testApp::postRequest(ofxHTTPServerResponse & response){
	if(response.url=="/postImage.of"){
		
        cout << response.uploadedFiles.begin()->first << "\t" << response.uploadedFiles.begin()->first << endl;
        posted_filename = response.uploadedFiles.begin()->first;
        
	}
}

void testApp::fileNotFound(ofxHTTPServerResponse & response) {

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    ofToggleFullscreen();
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

