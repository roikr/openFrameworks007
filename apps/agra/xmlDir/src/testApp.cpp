#include "testApp.h"
#include "ofxXmlSettings.h"
//#include "Poco/File.h"
//#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormatter.h"
//--------------------------------------------------------------
void testApp::setup(){
    visitDirectory("/Library/WebServer/Documents/test1");
    std::exit(0);
}

pair<double,time_t> testApp::visitDirectory(string path) {
    ofxXmlSettings xml;
    
    xml.addTag("files");
    xml.pushTag("files");
    ofDirectory dir;
    dir.listDir(path);
    double size = 0;
    time_t time = 0;
    int i=0;
    
    for (int j=0; j<dir.size(); j++) {
        if (dir.getName(j) != "files.xml") {
        
            xml.addTag("file");
            xml.addAttribute("file", "name", dir.getName(j),i);
            
            ofFile file = dir.getFile(j);
            double fileSize;
            time_t fileTime;
            
            if (file.isDirectory()) {
                xml.addAttribute("file", "dir",1,i);
                pair<int,time_t> p = visitDirectory(file.path());
                fileSize = p.first;
                fileTime = p.second;
                
            } else {
                fileSize=file.getSize();
                fileTime = file.getPocoFile().getLastModified().epochTime();
                
            }
            
            xml.addAttribute("file", "size",fileSize,i);
            
            Poco::Timestamp timestamp = Poco::Timestamp::fromEpochTime(fileTime);
            //cout << dir.getPath(j) << ": " << Poco::DateTimeFormatter::format(timestamp,"%d-%b-%Y %H:%M") << endl;
            xml.addAttribute("file", "date",Poco::DateTimeFormatter::format(timestamp,"%d-%b-%Y %H:%M"),i);
            
            size+=fileSize;
            time = max(time,fileTime);
            i++;
        }
    }
    
    xml.popTag();
    xml.saveFile(path+"/files.xml");
    
    cout << path << ": " << size << endl;
    return make_pair(size,time);
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