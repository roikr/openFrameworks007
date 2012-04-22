#pragma once

#include "ofMain.h"
#include <sstream>
#include "ofxXmlSettings.h"

struct file {
    file(string path,time_t time,bool directory) : path(path),time(time),directory(directory) {};
    
    string path;
    time_t time;
    bool directory;
};

class testApp : public ofBaseApp{

	public:
        void setup();
		void setup(string host,string root);
		void update();
        void draw() {};
    
        void start();
		
        void gotMessage(ofMessage msg);
    
        void urlResponse(ofHttpResponse &response);
    
        void parseDirXml(ofxXmlSettings &xml,string dirPath,vector<file>& files);

        void parseDir(file dir,ofBuffer &data);
    
    
   
    
    private:
    
    
    vector<file> list;
    vector<file> queue;
   
    string host;
    string root;
    
    
   
    
    long time;
    int iteration;
   		
};
