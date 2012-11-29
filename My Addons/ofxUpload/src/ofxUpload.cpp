//
//  ofxUpload.cpp
//  hindSight
//
//  Created by Roee Kremer on 11/29/12.
//
//

#include "ofxUpload.h"
#include "ofEvents.h"
#include "Poco/Net/FilePartSource.h"

using namespace std;

// ----------------------------------------------------------------------
ofxUpload::ofxUpload(){
    timeoutSeconds = 2;
    verbose = true;
    
    //start();
}
// ----------------------------------------------------------------------
ofxUpload::~ofxUpload(){
}


// ----------------------------------------------------------------------
void ofxUpload::upload(ofxFile file) {
	lock();
    files.push(file);
    unlock();
}

// ----------------------------------------------------------------------
void ofxUpload::start() {
    startThread(true, false);
    printf("thread started\n");
}
// ----------------------------------------------------------------------
void ofxUpload::stop() {
    stopThread();
    
}
// ----------------------------------------------------------------------
void ofxUpload::threadedFunction(){
    
    // loop through this process whilst thread running
    while( isThreadRunning() == true ){
		lock();
    	if(files.size()>0){
			ofxFile file = files.front();
	    	unlock();
			
            doUpload(file);
            printf("ofxUpload: (thread running) file submitted (post): %s\n", file.filename.c_str());
			
    		lock();
			files.pop();
	    	unlock();
    	}else{
    		unlock();
    	}
    	ofSleepMillis(10);
    }
    
}
// ----------------------------------------------------------------------
int ofxUpload::getQueueLength(){
    // i have taken this out for now
    return 0;
    /*if( lock() ){
     printf("get queue size\n");
     return forms.size();
     unlock();
     }*/
}
// ----------------------------------------------------------------------
void ofxUpload::clearQueue(){
    // i have taken this out for now
}


// ----------------------------------------------------------------------
ofxHttpResponse ofxUpload::doUpload(ofxFile &file){
	ofxHttpResponse response;
    try{
        URI uri( file.action.c_str() );
        std::string path(uri.getPathAndQuery());
        cout << "ofxUpload::post: "<< uri.getPathAndQuery() << endl;
        if (path.empty()) path = "/";
        
        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
        if(auth.getUsername()!="") auth.authenticate(req);
        
        if(sendCookies){
        	for(unsigned i=0; i<cookies.size(); i++){
        		NameValueCollection reqCookies;
        		reqCookies.add(cookies[i].getName(),cookies[i].getValue());
        		req.setCookies(reqCookies);
        	}
        }
        
        HTMLForm pocoForm;
        // create the form data to send
        
        pocoForm.setEncoding(HTMLForm::ENCODING_MULTIPART);
        
        pocoForm.addPart(file.filename,new FilePartSource(file.realname,file.filename,"application/octet-stream"));
        
        pocoForm.prepareSubmit(req);
        
        pocoForm.write(session.sendRequest(req));
        
        HTTPResponse res;
        istream& rs = session.receiveResponse(res);
        
		response = ofxHttpResponse(res, rs, path);
        
		if(sendCookies){
			cookies.insert(cookies.begin(),response.cookies.begin(),response.cookies.end());
		}
        
		if(response.status>=300 && response.status<400){
			Poco::URI uri(req.getURI());
			uri.resolve(res.get("Location"));
			response.location = uri.toString();
		}
        
    	ofNotifyEvent(newResponseEvent, response, this);
        
        
    }catch (Exception& exc){
        
    	std::cerr << "ofxUpload error--\n";
        
        //ofNotifyEvent(notifyNewError, "time out", this);
        
        // for now print error, need to broadcast a response
        std::cerr << exc.displayText() << std::endl;
        response.status = -1;
        response.reasonForStatus = exc.displayText();
        
    }
    
    return response;
}





// ----------------------------------------------------------------------
void ofxUpload::sendReceivedCookies(){
	sendCookies = true;
}

// ----------------------------------------------------------------------
void ofxUpload::setBasicAuthentication(string user, string password){
	auth.setUsername(user);
	auth.setPassword(password);
}

