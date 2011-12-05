#include "testApp.h"
#include "ofxXmlSettings.h"
#include <algorithm>

#ifdef TARGET_OF_IPHONE
#include "ofxiPhoneExtras.h"
#endif

#define SIMULTANEOUS_CONNECTIONS 10
//--------------------------------------------------------------
void testApp::setup(){
    ofRegisterURLNotification(this);
    host = "http://81.218.93.18/";
    root = "agra/";
    paths.push_back(root);
    //int res = ofLoadURLAsync(host+root);
}


string getDocumentsDirectory() {
#ifdef TARGET_OF_IPHONE
    return ofxiPhoneGetDocumentsDirectory();
#else
    return ofToDataPath("");
#endif
}


//--------------------------------------------------------------
void testApp::update(){
    
    if (queue.size()<SIMULTANEOUS_CONNECTIONS) {
        
        vector<string>::iterator iter = paths.begin();
        
        while (iter!=paths.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
           
           
            if (iter->at(iter->length()-1) !='/') {
                ofLoadURLAsync(host+*iter);
                queue.push_back(*iter);
                cout << "queue: " << queue.back() << endl;
                paths.erase(iter);
                iter = paths.begin();
            } else {
                iter++;
            }
        }
        
        iter = paths.begin();
        while (iter!=paths.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
            ofLoadURLAsync(host+*iter);
            queue.push_back(*iter);
            cout << "queue: " << queue.back() << endl;
            paths.erase(iter);
            iter=paths.begin();
        }
                
    }
    
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

void testApp::urlResponse(ofHttpResponse &response) {
    string url = response.request.name;
//    cout <<  url  << endl;
    
    if (response.status == 200 ) {
        string path = url.substr(host.length(),url.length()-1);
        
        vector<string>::iterator iter = find(queue.begin(),queue.end(),path);
        
        if (iter!=queue.end()) {
            queue.erase(iter);
        } else  {
            cout <<  "could not find "  << path << " in queue" << endl;
        }
        
//        
        
        if(path[path.length()-1] =='/') {
            cout << "folder: " << path  << endl;
            parseFolder(path,response.data);

        } else {
            cout << "file: " << path  << endl;
            ofBufferToFile(getDocumentsDirectory()+path, response.data,true);
        }
        
    
    } else
        cout << response.status << " " << response.error << endl;

}


void testApp::parseFolder(string path,ofBuffer &data) {
    cout << "parsing folder: " << path  << endl;
    
    vector<string> list;
    
    ofxXmlSettings xml;
    xml.loadFromBuffer(data);
    
    xml.pushTag("html");
    xml.pushTag("body");
    xml.pushTag("ul");
    
    cout << "remote folder" << endl;
    for (int i=0; i<xml.getNumTags("li"); i++) {
        string p = xml.getValue("li:a", "",i);
        list.push_back(path+p);
        cout << list.back() << endl;
    }
    
    xml.popTag();
    xml.popTag();
    xml.popTag();
    
    
    ofDirectory dir(getDocumentsDirectory()+path);
    if (!dir.exists()) {
        dir.create();
        cout << "new folder" << endl;
    } else {
        cout << "existing folder" << endl;

        dir.listDir();
        for(int i = 0; i < (int)dir.size(); i++){
            string lpath = dir.getPath(i);
            
            lpath = lpath.substr(getDocumentsDirectory().length(),lpath.length()-1);
            if (dir.getFile(i).isDirectory()) {
                lpath = lpath + "/";
            }
            cout << lpath;
            vector<string>::iterator iter = find(list.begin(),list.end(),lpath);
            
            if (iter!=list.end()) {
                cout << " - file exist in server" << endl;
            } else  {
                cout <<  " - deleting " << endl;
                dir.getFile(i).remove(true);
            }
        }
    }
    
    for (vector<string>::iterator iter=list.begin();iter!=list.end();iter++) {
        if(iter->at(iter->length()-1) =='/') {
            
            paths.push_back(*iter);
        } else {
            ofFile file(getDocumentsDirectory()+*iter);
            if (!file.exists()) {
                paths.push_back(*iter);
            }
        }
    }
    
    
}