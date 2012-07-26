#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	ofBackground(255,255,255);
	image.allocate(400,400,OF_IMAGE_COLOR);
	imageSaved  = false;

	server = ofxHTTPServer::getServer(); // get the instance of the server
	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir("upload");		 // folder to save uploaded files
	server->setCallbackExtension("of");	 // extension of urls that aren't files but will generate a post or get event
	ofAddListener(server->getEvent,this,&testApp::getRequest);
	ofAddListener(server->postEvent,this,&testApp::postRequest);
	server->start(8888);
}

void testApp::getRequest(ofxHTTPServerResponse & response){
	if(response.url=="/showScreen.of"){
		response.response="<html> <head> <title>oF http server</title> \
				<script> \
				function beginrefresh(){ \
					setTimeout(\"window.location.reload();\",30); \
				}\
		window.onload=beginrefresh; \
		</script>\
				</head> <body> <img src=\"screen.jpg\"/> </body> </html>";

		imageSaved  = false;
	}
}

void testApp::postRequest(ofxHTTPServerResponse & response){
	if(response.url=="/postImage.of"){
		postedImgName = response.requestFields["name"];
        for (map<string,string>::iterator iter = response.uploadedFiles.begin(); iter!=response.uploadedFiles.end();iter++) {
            cout << iter->first << "\t" << iter->second << endl;
        }
		postedImgFile = response.uploadedFiles.begin()->second ;//response.uploadedFiles[0];
//		response.response = "<html> <head> oF http server </head> <body> image " + response.uploadedFiles[0]  " received correctly <body> </html>";
        		response.response = "<html> <head> oF http server </head> <body> image " + postedImgFile +" received correctly <body> </html>";
	}
}

//--------------------------------------------------------------
void testApp::update(){
	for(int i=0; i<20; i++){
		radius[i].x = sin(ofGetElapsedTimef()+(float)(i*i))*100;
		radius[i].y = cos(ofGetElapsedTimef()-(float)(i*i))*100;
	}

	if(!imageSaved){
		image.grabScreen(150,150,300,300);
		image.saveImage("www/screen.jpg");
		imageSaved = true;
	}

	if(postedImgFile!=prevPostedImg){
		postedImg.loadImage("upload/" + postedImgFile);
		prevPostedImg = postedImgFile;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(200,190,0);
	ofFill();
	//ofSetPolyMode(OF_POLY_WINDING_ODD);
	ofBeginShape();
	for(int i=0; i<20; i++){
		ofCurveVertex(300+radius[i].x,300+radius[i].y);//,10,10);
	}
	ofEndShape(true);

	if(postedImgFile!=""){
		ofSetColor(0,0,0);
		ofDrawBitmapString(postedImgName,550,200);
		ofSetColor(255,255,255);
		postedImg.draw(550,220);
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

