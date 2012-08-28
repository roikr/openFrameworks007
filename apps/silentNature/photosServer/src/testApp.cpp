#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    ofSetWindowPosition(500, 0);
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	ofBackground(100);
	
    ofEnableAlphaBlending();

	server = ofxHTTPServer::getServer(); // get the instance of the server
//	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir("upload");		 // folder to save uploaded files
	server->setCallbackExtension("of");	 // extension of urls that aren't files but will generate a post or get event
	server->setListener(*this);
	server->start(8888);
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
		postedImgName = response.requestFields["name"];
        for (map<string,string>::iterator iter = response.uploadedFiles.begin(); iter!=response.uploadedFiles.end();iter++) {
            cout << iter->first << "\t" << iter->second << endl;
        }
		postedImgFile = response.uploadedFiles.begin()->second ;//response.uploadedFiles[0];
//		response.response = "<html> <head> oF http server </head> <body> image " + response.uploadedFiles[0]  " received correctly <body> </html>";
//        		response.response = "<html> <head> oF http server </head> <body> image " + postedImgFile +" received correctly <body> </html>";
	}
}

void testApp::fileNotFound(ofxHTTPServerResponse & response) {
    
}

//--------------------------------------------------------------
void testApp::update(){
	

	if(postedImgFile!=prevPostedImg){
		postedImg.loadImage("upload/" + postedImgFile);
		prevPostedImg = postedImgFile;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	

	if(postedImgFile!=""){
		ofSetColor(0,0,0);
		ofDrawBitmapString(postedImgName,0,0);
		ofSetColor(255,255,255);
		postedImg.draw(0,0);
	}


}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

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

