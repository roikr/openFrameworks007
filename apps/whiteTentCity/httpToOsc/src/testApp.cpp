#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    
        
    ofxXmlSettings xml;
    xml.loadFile("server.xml");
    xml.pushTag("server");
    
    sender.setup( xml.getAttribute("osc", "host", "localhost"), xml.getAttribute("osc", "port", 12345) );
    wordsSender.setup( xml.getAttribute("wordsSender", "host", "localhost"), xml.getAttribute("wordsSender", "port", 12346) );
    
    wordDuration = xml.getAttribute("animation", "wordDuration", 2000);
    messageRepetition = xml.getAttribute("animation", "messageRepetition", 3);
    
    
    xml.popTag();

    server = ofxHTTPServer::getServer(); // get the instance of the server
	server->setServerRoot("www");		 // folder with files to be served
	server->setUploadDir("upload");		 // folder to save uploaded files
	server->setCallbackExtension("asp");	 // extension of urls that aren't files but will generate a post or get event
	ofAddListener(server->getEvent,this,&testApp::getRequest);
//	ofAddListener(server->postEvent,this,&testApp::postRequest);
	server->start(8888);
}


void testApp::getRequest(ofxHTTPServerResponse & response){
	
    
    cout << response.url << endl;
    for (map<string,string>::iterator iter=response.requestFields.begin();iter!=response.requestFields.end();iter++) {
        cout << iter->first << ": " << iter->second << endl;
    }
    
    if (response.url == "/sendMessage.asp") {
        
        message msg;
        msg.tent = ofToInt(response.requestFields["tent"]);
        msg.str = response.requestFields["message"];
        msg.numWords=ofSplitString(msg.str, " ").size();
        
        cout << msg.str << endl;
        
        response.response="<html lang='he'> <head> <meta charset='utf-8' /> </head> <body> " + msg.str + " <body> </html>";
        
        queue.push_back(msg);
    
    }
    
    if (response.url == "/form.asp") {
        
        
        response.response="<html lang='he'> <head> <meta charset='utf-8' /> </head> <body dir='rtl'> <form  name='example' method='get' action='sendMessage.asp'>מספר אוהל:<input type='text' name='tent'><br/>הודעה:<input type='text' name='message'><br/> <input type='submit' name='submit1' value='שלח'> </form> <body> </html>";
        
        
        //response.response="<html> <head> oF http server </head> <body> " + response.url + " <body> </html>";
        
        
    }
    
    
}



//--------------------------------------------------------------
void testApp::update(){
    vector<message> newDisplay;
    
    
    for (vector<message>::iterator miter=display.begin(); miter!=display.end(); miter++) {
        // WORD_DURATION ms for each word
        if (ofGetElapsedTimeMillis()-miter->startTime<miter->numWords*wordDuration*messageRepetition) {
            newDisplay.push_back(*miter);
        } 
    }
    
    
    if (newDisplay.size()!=display.size()) {
        display = newDisplay;
    }
    
    vector<message> newQueue;
    
    for (vector<message>::iterator miter=queue.begin(); miter!=queue.end(); miter++) {
        vector<message>::iterator diter;
        for (diter=display.begin(); diter!=display.end() && miter->tent != diter->tent; diter++);
        if (diter==display.end()) {
            
            miter->startTime = ofGetElapsedTimeMillis();
            display.push_back(*miter);
            
            ofxOscMessage m;
            m.setAddress( "/sendMessage" );
            m.addIntArg(miter->tent);
            m.addStringArg(miter->str);
            sender.sendMessage( m );
            
            cout << "send: " << miter->tent << " " << miter->str << endl;
            
            m.clear();
            m.setAddress("/addWords");
            m.addStringArg(miter->str);
            wordsSender.sendMessage(m);
            
        } else {
            newQueue.push_back(*miter);
        }
    }
    
    
    if (newQueue.size()!=queue.size()) {
        queue = newQueue;
    }

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