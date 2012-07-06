#include "testApp.h"
#include "ofxURLFileLoader.h"

//--------------------------------------------------------------
void testApp::setup(){
    string url = "http://107.21.224.181/start/549453367/AAACtapZAgifcBAEGBY8KYPyxlFMSZAjdhE3jd2EEQSLF9W3RnaiPYuuFiIhOW1sj3B94d01SJJlUzIPGCyE8JkBLWMEpRxxnkM2M2TXAZDZD";
    
    ofxHttpResponse response = ofxLoadURL(url);
    
    //cout << response.data << endl;
    
    for (vector<pair<string, string> >::iterator iter=response.cookies.begin();iter!=response.cookies.end();iter++) {
        cout << iter->first << ": " << iter->second << endl;
    }
    
    string recom = "http://107.21.224.181/recommendations";
    ofxHttpRequest request(recom, response.cookies);
    
    cout << ofxLoadURL(request).data << endl;
    
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