#include "testApp.h"
//#include "Poco/File.h"
//#include "Poco/DateTimeParser.h"

//--------------------------------------------------------------

#define NEW_SIZE 128 // 1024

void testApp::setup(){
    
    extensions.push_back("png");
    extensions.push_back("jpeg");
    extensions.push_back("jpg");
    extensions.push_back("tif");
    visitDirectory("/Users/roikr/Developer/of_preRelease_v007_iphone/apps/saveAs/saveAs/bin/data/LIBRARY");
    
    std::exit(0);
}

void testApp::visitDirectory(string path) {
    
    
    ofDirectory dir;
    dir.listDir(path);
    
    for (int i=0; i<dir.size(); i++) {
        ofFile file = dir.getFile(i);
        
        if (file.isDirectory()) {
            visitDirectory(dir.getFile(i).path());
        } else {
            string extenstion = ofToLower(file.getExtension());
            
            for (vector<string>::iterator iter=extensions.begin(); iter!=extensions.end(); iter++) {
                if (*iter==extenstion) {
                    ofImage src;
                    src.loadImage(file.getAbsolutePath());
                    printf("%s:\t%.0fx%.0f",file.getAbsolutePath().c_str(),src.getWidth(),src.getHeight());
                    
                    int width,height;
                    
                    if (max(src.getWidth(),src.getHeight())>NEW_SIZE) {
                        if (src.getWidth()>=src.getHeight()) {
                            width = NEW_SIZE;
                            height = NEW_SIZE*src.getHeight()/src.getWidth();
                            
                        } else {
                            width = NEW_SIZE*src.getWidth()/src.getHeight();
                            height = NEW_SIZE;
                        }
                        printf(" -> %ix%i",width,height);
                        src.resize(width, height);
                        src.saveImage(file.getAbsolutePath());
                    }
                    printf("\n");
                    
                    break;
                }
            }

        }
        
                
        
        
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