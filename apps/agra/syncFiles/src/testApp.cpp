#include "testApp.h"
#include <algorithm>
#include "Poco/File.h"
#include "Poco/DateTimeParser.h"

#define SIMULTANEOUS_CONNECTIONS 10
#define TIME_BETWEEN_ITERATIONS 60 // minutes
#define TIME_DIFF_FOR_UPDATING 10


void testApp::setup() {
//    setup("http://81.218.93.18/","roee");
    setup("http://localhost/","test1");
    start();
}

//--------------------------------------------------------------
void testApp::setup(string host,string root){
    this->host = host;
    this->root = root;
    ofRegisterURLNotification(this);
    iteration = 0;
   
}

void testApp::start() {
    
    list.push_back(file(root,std::time(0),true));
    
    time = ofGetElapsedTimeMillis();
    iteration++;

    //int res = ofLoadURLAsync(host+root);
}

//--------------------------------------------------------------
void testApp::update(){
    
    if (queue.size()<SIMULTANEOUS_CONNECTIONS) {
        
        vector<file>::iterator iter = list.begin();
        
        while (iter!=list.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
           
            if (!iter->directory) { 
                ofLoadURLAsync(host+iter->path);
                queue.push_back(*iter);

                list.erase(iter);
                iter = list.begin();
            } else {
                iter++;
            }
        }
        
        iter = list.begin();
        while (iter!=list.end() && queue.size()<SIMULTANEOUS_CONNECTIONS) {
            ofLoadURLAsync(host+iter->path+"/files.xml");
            queue.push_back(*iter);

            list.erase(iter);
            iter=list.begin();
        }
          
        if (iteration && list.empty() && queue.empty() && ofGetElapsedTimeMillis()-time > TIME_BETWEEN_ITERATIONS * 60000) {
            start();
            
        }
    }
    
}




//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
   
}



void testApp::urlResponse(ofHttpResponse &response) {
    string url = response.request.name;
    //stream <<  url  << endl;
    
    if (response.status == 200 ) {
        string path = url.substr(host.length(),url.length());
        
        string lastComp = ofSplitString(path, "/").back();
        
        
        vector<file>::iterator iter;
        
        if (lastComp == "files.xml") {
            path = path.substr(0,path.length()-lastComp.length()-1);
        }
        
        for (iter=queue.begin();iter!=queue.end();iter++) {
            if (iter->path == path) {
                break;
            }
        }
        
        if (iter==queue.end()) {

            return;
        }
        
        file down = *iter;
        queue.erase(iter);
        
        
//        
        
        if(down.directory) { // path[path.length()-1] =='/'
            parseDir(down,response.data);

        } else {
            ofBufferToFile(ofToDataPath(path), response.data,true);
           
            ofFile(ofToDataPath(down.path)).getPocoFile().setLastModified(Poco::Timestamp::fromEpochTime(down.time)); // response.lastModified

            
        }
    } else {

    }
    

}


/*

pair<int,time_t> testApp::visitDirectory(string path) {
    ofxXmlSettings xml;
    
    xml.addTag("files");
    xml.pushTag("files");
    ofDirectory dir;
    dir.listDir(path);
    int size = 0;
    time_t time = 0;
    int i=0;
    
    for (int j=0; j<dir.size(); j++) {
        if (dir.getName(j) != "files.xml") {
            
            xml.addTag("file");
            xml.addAttribute("file", "name", dir.getName(j),i);
            
            ofFile file = dir.getFile(j);
            int fileSize;
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
*/

void testApp::parseDirXml(ofxXmlSettings &xml,string dirPath,vector<file>& files) {
    
    
    xml.pushTag("files");
    
    for (int i=0; i<xml.getNumTags("file"); i++) {
       
        string p = xml.getAttribute("file", "name", "",i);
        
        string lastModifiedStr = xml.getAttribute("file", "date", "",i);
        time_t lastModified = 0;
        Poco::DateTime dateTime;
        int diff;
        
        if (Poco::DateTimeParser::tryParse("%d-%b-%Y %H:%M",lastModifiedStr, dateTime, diff)) {
            lastModified = dateTime.timestamp().epochTime();
        }
        
        bool bDir = xml.getAttribute("file", "dir", 0,i);
        
        files.push_back(file(dirPath+"/"+p,lastModified,bDir)); // p.at(p.length()-1) == '/')
        
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

            }
        } else {
            list.push_back(*iter);
        }
    }

    
    remoteXml.saveFile(dir.path+"/files.xml");
}



