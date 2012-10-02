#include "testApp.h"

#define DELETE_AFTER 60 // minutes
#define NEW_IMAGE_DURATION 20000
#define IMAGE_DURATION 10000 // milis
//--------------------------------------------------------------
void testApp::setup(){
    ofHideCursor();
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

    imageNum = 0;
    postedImgFile = postedImgName = "";


    process();
    if (!images.empty()) {
        imageNum = rand() % images.size();
        image.loadImage(images[imageNum]);
        timer = ofGetElapsedTimeMillis() + IMAGE_DURATION;
    }

    bFullScreen = true;
}



//--------------------------------------------------------------
void testApp::update(){

    if (bFullScreen && ofGetElapsedTimeMillis()>5000) {
        bFullScreen = false;
        ofToggleFullscreen();
    }

    if(postedImgFile!="") {
        imageNum = images.size();
        images.push_back("upload/"+postedImgFile);
        timer = ofGetElapsedTimeMillis() + NEW_IMAGE_DURATION;
		image.loadImage(images.back());
		postedImgFile ="";
	}

    if (ofGetElapsedTimeMillis()>timer) {
        if (image.bAllocated()) {
            image.clear();
        }
        
        if (imageNum<images.size()) {
            images.erase(images.begin()+imageNum);
        }
        

        if (images.empty()) {
            process();
        }

        if (!images.empty()) {
            imageNum = rand() % images.size();
            image.loadImage(images[imageNum]);
            timer = ofGetElapsedTimeMillis() + IMAGE_DURATION;
        }
    }




}

//--------------------------------------------------------------
void testApp::draw(){

    image.draw(0,0);

}

void testApp::process(){
    images.clear();


    ofDirectory dir;
    dir.listDir("images");
    for (int i=0;i<dir.size();i++) {
        images.push_back(dir.getPath(i));
    }

    dir.listDir("upload");
    for (int i=0;i<dir.size();i++) {
        float diff = difftime(time(NULL),dir.getFile(i).getPocoFile().getLastModified().epochTime()) ;
        if (diff<DELETE_AFTER*60) {
            images.push_back(dir.getPath(i));
        } else {
            dir.getFile(i).remove();
        }
    }


    for (vector<string>::iterator iter = images.begin();iter != images.end(); iter++) {
        cout << *iter << endl;
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
    ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

