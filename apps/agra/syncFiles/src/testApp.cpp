#include "testApp.h"
#include <algorithm>
#include "Poco/File.h"
#include "Poco/DateTimeParser.h"

#define SIMULTANEOUS_CONNECTIONS 5
#define TIME_BETWEEN_ITERATIONS 15.0f // minutes
#define TIME_DIFF_FOR_UPDATING 10
#define MEASURE_INTERVAL 1000 // ms

//void testApp::setup() {
////    setup("http://81.218.93.18/","roee");
//    setup("http://localhost/","roee");
//    start();
//   
//}

//--------------------------------------------------------------
void testApp::setup(){
    
    ofRegisterURLNotification(this);
    bStarted = false;
    iteration = 0;
    start();
}

/*
void testApp::setURL(string host,string root) {
    this->host = host;
    this->root = root;
    iteration = 0;
    
    list.clear();
    queue.clear();
    bStarted = false;
    start();
    
}
*/
void testApp::start() {
    
    list.push_back(file(root,std::time(0),0,true));
    bStarted = true;
    totalBytes = 0;
    bytesReceived = 0;
    bytesUpdated = 0;
    metaBytes = 0;
    
    time = ofGetElapsedTimeMillis();
    
    measureTime = ofGetElapsedTimeMillis();
    bitrate = 0;
    bytesMeasure = 0;
    
    iteration++;

    //int res = ofLoadURLAsync(host+root);
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    if (floor((ofGetElapsedTimeMillis()-measureTime)/MEASURE_INTERVAL)) {
        bitrate = bytesMeasure*1000/MEASURE_INTERVAL*8.0;
        bytesMeasure = 0;
        measureTime = ofGetElapsedTimeMillis();
    }
    
    if (bStarted && list.empty() && queue.empty()) {
        bStarted = false;
        updateXml(root);
    }
    
    if (queue.size()<SIMULTANEOUS_CONNECTIONS) {
        
        vector<file>::iterator iter = list.begin();
        
        while (iter!=list.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
           
            if (!iter->directory) { 
                ofLoadURLAsync("http://"+host+"/"+iter->path);
                queue.push_back(*iter);

                list.erase(iter);
                iter = list.begin();
            } else {
                iter++;
            }
        }
        
        iter = list.begin();
        while (iter!=list.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
            ofLoadURLAsync("http://"+host+"/"+iter->path+"/files.xml");
            queue.push_back(*iter);

            list.erase(iter);
            iter=list.begin();
        }
          
        if (iteration && list.empty() && queue.empty() && ofGetElapsedTimeMillis()-time > TIME_BETWEEN_ITERATIONS * 60000) {
            start();
            
        }
    }
    
}

string testApp::getDebugStr() {
    string s;
    s = "bitrate:\t\t\t"+ofToString(bitrate/1000000,2) + " MBit\n";
    
    if (totalBytes) {
        s+="total:\t\t\t" + ofToString(totalBytes/1000000,0)+" MB\n";
        s+="time remained:\t\t\t"+ (bitrate ? ofToString((totalBytes-bytesReceived-bytesUpdated)*8.0/bitrate/60,0)+" min\n" : "unknown\n");
        s+="overhead:\t\t\t"+ofToString(metaBytes/1000,0)+" KB\n";
        
    }
    
    s+="last status:\t\t\t"+ofToString(status)+"\n";
    if (status!=200) {
        s+="last error:\t\t\t"+error+"\n";
    }
    
    return s;
}

void testApp::draw() {
    

    ofSetColor(0);
    
    ofDrawBitmapString("bitrate: "+ofToString(bitrate/1000000,2) + " MBit", 0,40);
    
    if (totalBytes) {
        ofSetColor(0, 255, 0);
        int updated = ofGetWidth()*bytesUpdated/totalBytes;
        ofRect(0, 0, updated, 20);
        ofSetColor(0, 0, 255);
        ofRect(updated, 0, ofGetWidth()*bytesReceived/totalBytes, 20);
        
        ofSetColor(0);
        ofDrawBitmapString("total: " + ofToString(totalBytes/1000000,0)+" MB", 0,60);
        ofDrawBitmapString("time remained: "+ (bitrate ? ofToString((totalBytes-bytesReceived-bytesUpdated)*8.0/bitrate/60,0)+" min" : "unknown"), 0,80);
        ofDrawBitmapString("overhead: "+ofToString(metaBytes/1000,0)+" KB",0,100);
        
    }

    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
   
}



void testApp::urlResponse(ofHttpResponse &response) {
    string url = response.request.name;
    
    
    //stream <<  url  << endl;
    
    //list.push_back(*iter);
    
    status = response.status;
    
   
    string path = url.substr(("http://"+host+"/").length(),url.length());
    
    
    bool bFilesXml = false;
    string lastComp = ofSplitString(path, "/").back();

    if (lastComp == "files.xml") {
        bFilesXml = true;
        path = path.substr(0,path.length()-lastComp.length()-1);
        metaBytes += response.data.size();
    }
        
        
    vector<file>::iterator iter;
    
    for (iter=queue.begin();iter!=queue.end();iter++) {
        if (iter->path == path) {
            break;
        }
    }
    
    if (iter==queue.end()) {
        status = -1;
        error = "could not find "+path+" in the queue";
    } else {
        file down = *iter;
        queue.erase(iter);
        
        switch (response.status) {
            case 200: {
                bytesMeasure+=response.data.size();
                
                
                if(down.directory) { // path[path.length()-1] =='/'
                    parseDir(down,response.data);
                    
                } else {
                    ofBufferToFile(ofToDataPath(path), response.data,true);
                    
                    ofFile(ofToDataPath(down.path)).getPocoFile().setLastModified(Poco::Timestamp::fromEpochTime(down.time)); // response.lastModified
                    bytesReceived += down.size;
                }

            } break;
                
            case -1:
                error = response.error;
                list.push_back(down);
                break;
                
            default:
                error = response.error;
                break;
        }
        
    }
    
}



void testApp::parseDirXml(ofxXmlSettings &xml,string dirPath,vector<file>& files) {
    
    
    xml.pushTag("files");
    
    for (int i=0; i<xml.getNumTags("file"); i++) {
        
        
        string p = xml.getAttribute("file", "name", "",i);
        double size = xml.getAttribute("file", "size", 0.0,i);
        
        string lastModifiedStr = xml.getAttribute("file", "date", "",i);
        time_t lastModified = 0;
        Poco::DateTime dateTime;
        int diff;
        
        if (Poco::DateTimeParser::tryParse("%d-%b-%Y %H:%M",lastModifiedStr, dateTime, diff)) {
            lastModified = dateTime.timestamp().epochTime();
        }
        
        bool bDir = xml.getAttribute("file", "dir", 0,i);
        
        files.push_back(file(dirPath+"/"+p,lastModified,size,bDir)); // p.at(p.length()-1) == '/')
        
    }
    
    xml.popTag();
    
}
 
void testApp::parseDir(file dir,ofBuffer &data) {
    
    
    
    cout << "parsing: " << dir.path << endl;
    //stream << data;
    ofxXmlSettings remoteXml;
    vector<file> remote;
    remoteXml.loadFromBuffer(data);
    parseDirXml(remoteXml, dir.path, remote);
    
    if (dir.path == root) {
        for (vector<file>::iterator iter=remote.begin(); iter!=remote.end(); iter++) {
            totalBytes+=iter->size;
        }
    }
    
   
    ofxXmlSettings localXml;
    vector<file> local;
    bool bLocalLoaded = localXml.loadFile(dir.path+"/files.xml");
    if (bLocalLoaded) {
        parseDirXml(localXml, dir.path, local);
    }
    
    
    cout << "remote directory: " << dir.path  << endl;
    for (vector<file>::iterator iter = remote.begin();iter!=remote.end();iter++) {
         cout << iter->path << (iter->directory ? "\tdirectory" : "") << "\tlastModified: " << ctime(&(iter->time)) ;
    }

    
    ofDirectory localDir(ofToDataPath(dir.path));
    if (!localDir.exists()) {
        
        localDir.create();
        
        // roikr: can't setLastModified on folder because we will change it child later so it lastUpdated will change...
        // local.getPocoFile().setLastModified(Poco::Timestamp::fromEpochTime(dir.time)); // response.lastModified
    
        cout << "creating new folder " << dir.path << " at " << ctime(&dir.time) << endl;
    } else {
        
        localDir.listDir();
        for(int i = 0; i < (int)localDir.size(); i++){
            if (localDir.getName(i)!="files.xml") {
                string lpath = localDir.getPath(i);
                
                lpath = lpath.substr(ofToDataPath("").length(),lpath.length()-1);
                
                
                vector<file>::iterator iter;
                for (iter=remote.begin();iter!=remote.end();iter++) {
                    if (iter->path == lpath) {
                        break;
                    }
                }
                
                if (iter==remote.end()) {
                    localDir.getFile(i).remove(true);

                    cout << lpath << " does not exist in remote server - deleting" <<endl;
                    
                } 

            }
        }
    }
    
    for (vector<file>::iterator iter=remote.begin();iter!=remote.end();iter++) {
        if(ofFile(ofToDataPath(iter->path)).exists()) {
            time_t time = 0;
            if (iter->directory) {
                if (bLocalLoaded) {
                    vector<file>::iterator liter;
                    for (liter=local.begin();liter!=local.end();liter++) {
                        if (iter->path==liter->path) {
                            break;
                        }
                        
                    }
                    if (liter!=local.end()) {
                        time = liter->time;
                    }
                }
            } else {
                time = ofFile(ofToDataPath(iter->path)).getPocoFile().getLastModified().epochTime();
            }
            double diff = difftime(time, iter->time);
            if (ABS(diff) > TIME_DIFF_FOR_UPDATING) {
                list.push_back(*iter);

                cout << iter->path << " is not updated" <<endl;

            } else {
                bytesUpdated += iter->size;
            }
        } else {
            list.push_back(*iter);
            cout << iter->path << " is not exist" <<endl;
        }
    }

    
    remoteXml.saveFile(dir.path+"/new_files.xml");
}


void testApp::updateXml(string path) {
    
    cout << "updateXml: " << path << endl;
    
    ofDirectory dir;
    dir.listDir(path);
   
    
    for (int j=0; j<dir.size(); j++) {
        if (dir.getName(j) != "files.xml" && dir.getName(j) != "new_files.xml") {
            if (dir.getFile(j).isDirectory()) {
                updateXml(dir.getPath(j));
                                
            }         
        }
    }
    
    ofFile file = ofFile(path+"/new_files.xml");
    if (file.exists()) {
        file.moveTo(path+"/files.xml",true,true);
    }
    
}

