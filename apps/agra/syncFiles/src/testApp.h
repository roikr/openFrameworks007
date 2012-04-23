#pragma once

#include "ofMain.h"
#include <sstream>
#include "ofxXmlSettings.h"

struct file {
    file(string path,time_t time,double size,bool directory) : path(path),time(time),size(size),directory(directory) {};
    
    string path;
    time_t time;
    double size;
    bool directory;
};

class testApp : public ofBaseApp{

	public:
        void setup();
		void setup(string host,string root);
		void update();
        void draw();
    
        void start();
		
        void gotMessage(ofMessage msg);
    
        void urlResponse(ofHttpResponse &response);
    
        void parseDirXml(ofxXmlSettings &xml,string dirPath,vector<file>& files);

        void parseDir(file dir,ofBuffer &data);
    
        void updateXml(string path); // second pass - update xml files
    
    private:
    
    
    vector<file> list;
    vector<file> queue;
   
    string host;
    string root;
    
    
   
    
    long time;
    int iteration;
    
    bool bStarted;
    
    double totalBytes;
    double bytesReceived;
    double bytesUpdated;
    double metaBytes;
    
    int measureTime;
    unsigned int bytesMeasure;
    unsigned int lastMeasure;
   		
};
