#include "testApp.h"
//#include "Poco/File.h"
//#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormatter.h"
//--------------------------------------------------------------
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
                    int size = ofNextPow2(max(src.getWidth(),src.getHeight()));
                    printf("%s:\t%.0fx%.0f -> %i\n",file.getAbsolutePath().c_str(),src.getWidth(),src.getHeight(),size);
                    
                    ofImage dest;
                    dest.allocate(size, size, src.getPixelsRef().getImageType());
                    
                    for	(int j=0;j< src.getHeight();j++) {
                        for (int i=src.getWidth(); i<size; i++) {
                            dest.setColor(i, j, ofColor(0,0,0,0));
                        }
                    }
                    
                    for	(int j=src.getHeight();j< size;j++) {
                        for (int i=0; i<size; i++) {
                            dest.setColor(i, j, ofColor(0,0,0,0));
                        }
                    }
                    
                    
                    
                    for	(int j=0;j< src.getHeight();j++) {
                        for (int i=0; i<src.getWidth(); i++) {
                            dest.setColor(i, j, src.getColor(i,j));
                        }
                    }
                    
                    file.remove();
                    dest.saveImage(file.getEnclosingDirectory()+"/"+file.getBaseName()+".png");
                    
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