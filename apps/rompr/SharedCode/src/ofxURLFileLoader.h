#pragma once

#include <deque>
#include <queue>

#include "ofThread.h"
#include "ofEvents.h"
#include "ofFileUtils.h"

#include "Poco/Condition.h"


class ofxHttpRequest{
public:
	ofxHttpRequest(){};
    
    ofxHttpRequest(string url,string name,bool saveTo=false)
	:url(url)
	,name(name)
	,saveTo(saveTo)
	,id(nextID++){}


	ofxHttpRequest(string url,vector<pair<string,string> > cookies,bool saveTo=false)
	:url(url)
	,name(url)
    ,cookies(cookies)
	,saveTo(saveTo)
	,id(nextID++){}

	string				url;
	string				name;
	bool				saveTo;

	int getID(){return id;}
    
    vector<pair<string,string> > cookies;
private:
	int					id;
	static int			nextID;
};

class ofxHttpResponse{
public:
	ofxHttpResponse(){}

	ofxHttpResponse(ofxHttpRequest request,vector<pair<string,string> > cookies,const ofBuffer & data,int status, string error)
	:request(request)
	,data(data)
	,status(status)
	,error(error)
    ,cookies(cookies){}

	ofxHttpResponse(ofxHttpRequest request,vector<pair<string,string> > cookies,int status,string error)
	:request(request)
	,status(status)
	,error(error)
    ,cookies(cookies){}

	operator ofBuffer&(){
		return data;
	}

	ofxHttpRequest	    request;
	ofBuffer		    data;
	int					status;
	string				error;
    vector<pair<string,string> > cookies;
};

ofxHttpResponse ofxLoadURL(string url);
ofxHttpResponse ofxLoadURL(ofxHttpRequest request);
int ofxLoadURLAsync(string url, string name=""); // returns id
int ofxLoadURLAsync(ofxHttpRequest request); // returns id
ofxHttpResponse ofxSaveURLTo(string url, string path);
int ofxSaveURLAsync(string url, string path);
void ofxRemoveURLRequest(int id);
void ofxRemoveAllURLRequests();

extern ofEvent<ofxHttpResponse> ofxURLResponseEvent;

template<class T>
void ofxRegisterURLNotification(T * obj){
	ofAddListener(ofxURLResponseEvent,obj,&T::urlResponse);
}

template<class T>
void ofxUnregisterURLNotification(T * obj){
	ofRemoveListener(ofxURLResponseEvent,obj,&T::urlResponse);
}


class ofxURLFileLoader : public ofThread  {

    public:

        ofxURLFileLoader();
        ofxHttpResponse get(string url);
        ofxHttpResponse get(ofxHttpRequest request);
        int getAsync(string url, string name=""); // returns id
        int getAsync(ofxHttpRequest request); // returns id
        ofxHttpResponse saveTo(string url, string path);
        int saveAsync(string url, string path);
		void remove(int id);
		void clear();

    protected:

		// threading -----------------------------------------------
		void threadedFunction();
        void start();
        void stop();
        void update(ofEventArgs & args);  // notify in update so the notification is thread safe

    private:

		// perform the requests on the thread
        ofxHttpResponse handleRequest(ofxHttpRequest request);

		deque<ofxHttpRequest> requests;
		queue<ofxHttpResponse> responses;

		Poco::Condition condition;

};
