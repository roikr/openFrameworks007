//
//  ofxUpload.h
//  hindSight
//
//  Created by Roee Kremer on 11/29/12.
//
//

#pragma once

#include "ofMain.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/Mutex.h"

#include <iostream>
#include <queue>
#include <istream>

using namespace Poco::Net;
using namespace Poco;

class ofxHttpListener;
class ofxHttpEventManager;

struct ofxHttpResponse{
	ofxHttpResponse(HTTPResponse& pocoResponse, std::istream &bodyStream, string turl, bool binary=false){
		status=pocoResponse.getStatus();
		timestamp=pocoResponse.getDate();
		reasonForStatus=pocoResponse.getReasonForStatus(pocoResponse.getStatus());
		contentType = pocoResponse.getContentType();
		responseBody.set(bodyStream);
        
        url = turl;
        pocoResponse.getCookies(cookies);
	}
    
	ofxHttpResponse(){}
    
	string getURLFilename(){
		return url.substr(url.rfind('/')+1);
	}
    
	int status; 				// return code for the response ie: 200 = OK
	string reasonForStatus;		// text explaining the status
	ofBuffer responseBody;		// the actual response
	string contentType;			// the mime type of the response
	Poco::Timestamp timestamp;		// time of the response
	string url;
	vector<HTTPCookie> cookies;
	string location;
};

struct ofxFile {
    ofxFile(string filename,string realname,string action):filename(filename),realname(realname),action(action) {};
    string filename;
    string realname;
    string action;
};

class ofxUpload : public ofThread{
    
public:
    
    ofxUpload();
    ~ofxUpload();
    //-------------------------------
    // non blocking functions
    
    void upload(ofxFile file);
    
    
    int getQueueLength();
    void clearQueue();
    
    //-------------------------------
    // threading stuff
    void threadedFunction();
    
    //------------------------------
    // events
    ofEvent<ofxHttpResponse> newResponseEvent;
    
    // other stuff-------------------
    void setTimeoutSeconds(int t){
        timeoutSeconds = t;
    }
    void setVerbose(bool v){
        verbose = v;
    }
    
    void sendReceivedCookies();
    
    void setBasicAuthentication(string user, string password);
    
    
    void start();
    void stop();
    
protected:
    
    ofxHttpResponse doUpload(ofxFile & file);
    
    bool verbose;
    int timeoutSeconds;
    bool sendCookies;
    
    
    std::queue<ofxFile> files;
    vector<HTTPCookie> cookies;
    HTTPBasicCredentials auth;
    
    
};

