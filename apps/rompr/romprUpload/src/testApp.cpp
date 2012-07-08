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

#include "cJSON.h"

using namespace Poco::Net;
using namespace Poco;

const string HOST_NAME = "107.21.224.181";
const string USER_ID = "549453367";
const string FB_ACCESS_TOKEN = "AAACtapZAgifcBAG1nd96WupL2vU103yrSCsmUA9KX0ElPYISwqhYcAco7W3BSh8NSha0qkDvMjp3xfUE2W1AHA3apk2rmlIYHstZAMwQZDZD";
//--------------------------------------------------------------

ofxHttpResponse postForm(string action,vector <string> &formIds,vector <string> &formValues,std::map<string,string> &formFiles,vector<pair<string,string> > &cookies) {
    
    ofxHttpRequest request(action,action);
    ofxHttpResponse response;
    response.request = request;
    try{
        URI uri( action.c_str() );
        std::string path(uri.getPathAndQuery());
        cout << "ofxHttpUtils::post: "<< uri.getPathAndQuery() << endl;
        if (path.empty()) path = "/";
        
        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
        //        if(auth.getUsername()!="") auth.authenticate(req);
        
        if(!cookies.empty()){
            NameValueCollection reqCookies;
        	for(unsigned i=0; i<cookies.size(); i++){
        		reqCookies.add(cookies[i].first,cookies[i].second);
        	}
            req.setCookies(reqCookies);
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
        
        response.data = rs;
        response.status = res.getStatus();
        response.error = res.getReason();
        
        
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
        
        response.status = -1;
        response.error = exc.displayText();
        
    }
    
    return response;

    
}

void testApp::setup(){
    string url = "http://"+HOST_NAME+"/mobile/start/"+USER_ID+"/"+FB_ACCESS_TOKEN;
    
    ofxHttpResponse loginRes = ofxLoadURL(url);
    
    cout << "login, status: " << loginRes.status << ", response: " << loginRes.data.getText() << endl;
    
//    for (vector<pair<string, string> >::iterator iter=loginRes.cookies.begin();iter!=loginRes.cookies.end();iter++) {
//        cout << iter->first << ": " << iter->second << endl;
//    }
    
   
//    string recomStr = "http://107.21.224.181/recommendations";
//    ofxHttpResponse recomRes = ofxLoadURL(ofxHttpRequest(recomStr,loginRes.cookies)); // , 
//    cout << recomRes.data.getText() << endl;
    
    

    
    vector <pair<string,string> > nvc;
    std::map<string,string> files;
    
    files["qqfile"]=ofToDataPath("idan.jpg");
    
    //ofxHttpResponse response =  postForm("http://"+HOST_NAME+"/ajax/upload_image",   formIds , formValues, formFiles, loginRes.cookies);
    ofxHttpResponse response = ofxLoadURL(ofxHttpRequest("http://"+HOST_NAME+"/ajax/upload_image", nvc, files, loginRes.cookies));
    
    cout << "status: " << response.status << ", reason: " << response.error << endl;
    cout << response.data.getText() << endl;
    
    string image_path;
    cJSON *result = cJSON_Parse(response.data.getText().c_str());
	if ( result )
    {
        cJSON *path = cJSON_GetObjectItem(result,"image_path");
        image_path = path->valuestring;
       
    }
    
    cout << "image_path: " << image_path << endl;
    files.clear();
    nvc.push_back(make_pair("mode", "new"));
    nvc.push_back(make_pair("id", "0"));
    nvc.push_back(make_pair("title", "ponding"));
    nvc.push_back(make_pair("free_text", "indeed"));
    nvc.push_back(make_pair("creation_date", ""));
    nvc.push_back(make_pair("modification_date", ""));
    nvc.push_back(make_pair("category", "1"));
    nvc.push_back(make_pair("age_from", ""));
    nvc.push_back(make_pair("age_to", ""));
    nvc.push_back(make_pair("location", "mobile_location"));
    nvc.push_back(make_pair("location_longitude", "34.790130615234"));
    nvc.push_back(make_pair("location_latitude", "32.085258483887"));
    nvc.push_back(make_pair("location_accuracy", "true"));
    nvc.push_back(make_pair("uploaded_image_path", image_path));
    
   
        
//    response =  postForm("http://"+HOST_NAME+"/recommendation/post",   formIds , formValues, formFiles, loginRes.cookies);    
    response = ofxLoadURL(ofxHttpRequest("http://"+HOST_NAME+"/recommendation/post", nvc, files, loginRes.cookies));
    
    cout << "status: " << response.status << ", reason: " << response.error << endl;
//    cout << response.data.getText();
    
    
    
    
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