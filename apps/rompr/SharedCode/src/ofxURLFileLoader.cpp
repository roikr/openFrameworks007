#include "ofxURLFileLoader.h"
#include "ofAppRunner.h"
#include "ofUtils.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/Net/HTTPStreamFactory.h"

#include "Poco/Net/HTTPCookie.h"
#include "Poco/Net/NameValueCollection.h"

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPStreamFactory;
using Poco::Net::HTTPCookie;
using Poco::Net::NameValueCollection;

using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::URIStreamOpener;
using Poco::Exception;



#include "ofConstants.h"

static bool factoryLoaded = false;
int	ofxHttpRequest::nextID = 0;
ofEvent<ofxHttpResponse> ofxURLResponseEvent;

ofxURLFileLoader::ofxURLFileLoader() {
	if(!factoryLoaded){
		try {
			HTTPStreamFactory::registerFactory();
			factoryLoaded = true;
		}
		catch (Poco::SystemException PS) {
			ofLog(OF_LOG_ERROR, "Got exception in url ofxURLFileLoader");
		}
	}
}

ofxHttpResponse ofxURLFileLoader::get(string url) {
    ofxHttpRequest request(url,url);
    return handleRequest(request);
}

ofxHttpResponse ofxURLFileLoader::get(ofxHttpRequest request) {
    return handleRequest(request);
}


int ofxURLFileLoader::getAsync(string url, string name){
	if(name=="") name=url;
	ofxHttpRequest request(url,name);
	lock();
	requests.push_back(request);
	unlock();
	start();
	return request.getID();
}


ofxHttpResponse ofxURLFileLoader::saveTo(string url, string path){
    ofxHttpRequest request(url,path,true);
    return handleRequest(request);
}

int ofxURLFileLoader::saveAsync(string url, string path){
	ofxHttpRequest request(url,path,true);
	lock();
	requests.push_back(request);
	unlock();
	start();
	return request.getID();
}

void ofxURLFileLoader::remove(int id){
	lock();
	for(int i=0;i<(int)requests.size();i++){
		if(requests[i].getID()==id){
			requests.erase(requests.begin()+i);
			return;
		}
	}
	unlock();
}

void ofxURLFileLoader::clear(){
	lock();
	requests.clear();
	while(!responses.empty()) responses.pop();
	unlock();
}

void ofxURLFileLoader::start() {
     if (isThreadRunning() == false){
        startThread(false, false);   // blocking, verbose
    }else{
    	ofLog(OF_LOG_VERBOSE,"signaling new request condition");
    	condition.signal();
    }
}

void ofxURLFileLoader::stop() {
    stopThread();
}

void ofxURLFileLoader::threadedFunction() {
	while( isThreadRunning() == true ){
		lock();
    	ofLog(OF_LOG_VERBOSE,"starting thread loop ");
		if(requests.size()>0){
	    	ofLog(OF_LOG_VERBOSE,"querying request " + requests.front().name);
			ofxHttpRequest request(requests.front());
			unlock();

			ofxHttpResponse response(handleRequest(request));

			if(response.status!=-1){
				lock();
		    	ofLog(OF_LOG_VERBOSE,"got request " + requests.front().name);
				responses.push(response);
				requests.pop_front();
		    	ofAddListener(ofEvents.update,this,&ofxURLFileLoader::update);
				unlock();
			}else{
		    	ofLog(OF_LOG_VERBOSE,"failed getting request " + requests.front().name);
			}
			ofSleepMillis(10);
		}else{
			ofLog(OF_LOG_VERBOSE,"stopping on no requests condition");
			condition.wait(mutex);
		}
	}
}

ofxHttpResponse ofxURLFileLoader::handleRequest(ofxHttpRequest request) {
	try {
		URI uri(request.url);
		std::string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		HTTPClientSession session(uri.getHost(), uri.getPort());
		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		session.setTimeout(Poco::Timespan(20,0));
        
        if (!request.cookies.empty()) {
            NameValueCollection mvc;
//            cout << "request cookies:" << endl;
            for (vector<pair<string,string> >::iterator iter = request.cookies.begin();iter!=request.cookies.end();iter++) {
                mvc.add(iter->first, iter->second);
//                cout << iter->first << ": " << iter->second << endl;
                
            }
            req.setCookies(mvc);
            
        }
        
		session.sendRequest(req);
		HTTPResponse res;
		istream& rs = session.receiveResponse(res);
        
        vector<HTTPCookie> pocoCookies;
        res.getCookies(pocoCookies);
        vector<pair<string,string> > cookies;
        
//        res.write(cout);
        
        for (vector<HTTPCookie>::iterator iter=pocoCookies.begin();iter!=pocoCookies.end();iter++) {
            cookies.push_back(make_pair(iter->getName(), iter->getValue()));
        }
        
		if(!request.saveTo){
			return ofxHttpResponse(request,cookies,rs,res.getStatus(),res.getReason());
		}else{
			ofFile saveTo(request.name,ofFile::WriteOnly);
			char aux_buffer[1024];
			rs.read(aux_buffer, 1024);
			std::streamsize n = rs.gcount();
			while (n > 0){
				// we resize to size+1 initialized to 0 to have a 0 at the end for strings
				saveTo.write(aux_buffer,n);
				if (rs){
					rs.read(aux_buffer, 1024);
					n = rs.gcount();
				}
				else n = 0;
			}
			return ofxHttpResponse(request,cookies,res.getStatus(),res.getReason());
		}

	} catch (Exception& exc) {
        ofLog(OF_LOG_ERROR, "ofxURLFileLoader " + exc.displayText());
        vector<pair<string,string> > cookies;
        return ofxHttpResponse(request,cookies,-1,exc.displayText());
    }	
	
}	

void ofxURLFileLoader::update(ofEventArgs & args){
	lock();
	if(responses.size()){
		ofxHttpResponse response(responses.front());
		ofLog(OF_LOG_VERBOSE,"ofURLLoader::update: new response " +response.request.name);
		responses.pop();
		unlock();
		ofNotifyEvent(ofxURLResponseEvent,response);
	}else{
		ofRemoveListener(ofEvents.update,this,&ofxURLFileLoader::update);
		unlock();
	}

}

static ofxURLFileLoader & ofxGetFileLoader(){
	static ofxURLFileLoader * fileLoader = new ofxURLFileLoader;
	return *fileLoader;
}

ofxHttpResponse ofxLoadURL(string url){
	return ofxGetFileLoader().get(url);
}

ofxHttpResponse ofxLoadURL(ofxHttpRequest request){
	return ofxGetFileLoader().get(request);
}

int ofxLoadURLAsync(string url, string name){
	return ofxGetFileLoader().getAsync(url,name);
}

ofxHttpResponse ofxSaveURLTo(string url, string path){
	return ofxGetFileLoader().saveTo(url,path);
}

int ofxSaveURLAsync(string url, string path){
	return ofxGetFileLoader().saveAsync(url,path);
}

void ofxRemoveURLRequest(int id){
	ofxGetFileLoader().remove(id);
}

void ofxRemoveAllURLRequests(){
	ofxGetFileLoader().clear();
}
