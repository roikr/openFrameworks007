#include "testApp.h"
#include "ofxURLFileLoader.h"

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

#include "Poco/Net/FilePartSource.h"

using namespace Poco::Net;
using namespace Poco;

const string HOST_NAME = "107.21.224.181";
const string USER_ID = "549453367";
const string FB_ACCESS_TOKEN ="AAACtapZAgifcBALkk9e7p6RYZAYm6hkZCXKub5tdRrD9DyBASA2oQx0xipZBpZBHwL4jIGZA5WjiZBlugZB5QQ6ak7fz0JLyKV8LxDx1sijoWwZDZD";

//--------------------------------------------------------------
void testApp::setup(){
    string url = "http://"+HOST_NAME+"/mobile/start/"+USER_ID+"/"+FB_ACCESS_TOKEN;
    
    ofxHttpResponse loginRes = ofxLoadURL(url);
    
    cout << loginRes.data.getText() << endl;
    
//    for (vector<pair<string, string> >::iterator iter=loginRes.cookies.begin();iter!=loginRes.cookies.end();iter++) {
//        cout << iter->first << ": " << iter->second << endl;
//    }
    
   
//    ofxHttpResponse recomRes = ofxLoadURL(ofxHttpRequest("http://107.21.224.181/recommendations", loginRes.cookies));
//    cout << recomRes.data.getText() << endl;
    
    string action = "http://107.21.224.181/recommendation/post";
    bool sendCookies = false;
    vector <string> formIds;
	vector <string> formValues;
	std::map<string,string> formFiles;
    
    
    
    formIds.push_back("mode" );
    formValues.push_back("new");
    formIds.push_back("title" );
    formValues.push_back("ponding");
    formIds.push_back("free_test" );
    formValues.push_back("indeed");
    formFiles["file"]=ofToDataPath("idan.jpg");
        
    ofxHttpRequest request(action,action);
    ofxHttpResponse response;
    try{
        URI uri( action.c_str() );
        std::string path(uri.getPathAndQuery());
        cout << "ofxHttpUtils::post: "<< uri.getPathAndQuery() << endl;
        if (path.empty()) path = "/";
        
        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
//        if(auth.getUsername()!="") auth.authenticate(req);
        
        if(sendCookies){
        	for(unsigned i=0; i<loginRes.cookies.size(); i++){
        		NameValueCollection reqCookies;
        		reqCookies.add(loginRes.cookies[i].first,loginRes.cookies[i].second);
        		req.setCookies(reqCookies);
        	}
        }
        
        HTMLForm pocoForm;
        // create the form data to send
        if(formFiles.size()>0)
        	pocoForm.setEncoding(HTMLForm::ENCODING_MULTIPART);
        else
        	pocoForm.setEncoding(HTMLForm::ENCODING_URL);
        
        // form values
        for(unsigned i=0; i<formIds.size(); i++){
            const std::string name = formIds[i].c_str();
            const std::string val = formValues[i].c_str();
            pocoForm.set(name, val);
        }
        
        map<string,string>::iterator it;
        for(it = formFiles.begin(); it!=formFiles.end(); it++){
        	string fileName = it->second.substr(it->second.find_last_of('/')+1);
        	cout << "adding file: " << fileName << " path: " << it->second << endl;
        	pocoForm.addPart(it->first,new FilePartSource(it->second));
        }
        
        pocoForm.prepareSubmit(req);
        
        pocoForm.write(session.sendRequest(req));
        
        HTTPResponse res;
        istream& rs = session.receiveResponse(res);
        
        
        
        response = ofxHttpResponse(request,loginRes.cookies,rs,res.getStatus(),res.getReason());
        
//		response = ofxHttpResponse(res, rs, path);
//        
//		if(sendCookies){
//			cookies.insert(cookies.begin(),response.cookies.begin(),response.cookies.end());
//		}
//        
		if(response.status>=300 && response.status<400){
			Poco::URI uri(req.getURI());
			uri.resolve(res.get("Location"));
            
            cout << "status: " << response.status << ", location: " << uri.toString() << endl;
//			response.location = uri.toString();
		}
        
//    	ofNotifyEvent(newResponseEvent, response, this);
        
        
    }catch (Exception& exc){
        
    	std::cerr << "ofxHttpUtils error--\n";
        
        //ofNotifyEvent(notifyNewError, "time out", this);
        
        // for now print error, need to broadcast a response
        std::cerr << exc.displayText() << std::endl;
//        response.status = -1;
//        response.reasonForStatus = exc.displayText();
        
    }
    
    cout << "status: " << response.status << ", reason: " << response.error << endl;
    cout << response.data.getText();
    
    
    
    
    //ofxHttpRequest(recom, response.cookies);
    
    //cout << ofxLoadURL(ofxHttpRequest(recom, response.cookies)).data << endl;
    
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