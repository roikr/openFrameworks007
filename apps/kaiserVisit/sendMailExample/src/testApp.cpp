#include "testApp.h"

#include "Poco/Net/MailMessage.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"

#include "Poco/Exception.h"

using Poco::Net::SMTPClientSession;
using Poco::Exception;

#define OFX_SMTP_HOST "localhost"
#define OFX_SMTP_PORT 25

//--------------------------------------------------------------
void testApp::setup(){
    SMTPClientSession * session;
    
    try{
		session=new Poco::Net::SMTPClientSession(OFX_SMTP_HOST,OFX_SMTP_PORT);
//		session->login(Poco::Net::SMTPClientSession::AUTH_LOGIN,"roikr","kremer75");
        session->login();
        
        Poco::Timestamp date;
        string sender;
        string subject;
        string contentType;
        string content;
        vector<string> recipients;
        vector<string> cc;
        vector<string> bcc;
        vector<string> attachmentPaths;
        
        recipients.push_back("roikr75@gmail.com");
        sender="roikr75@gmail.com";
        subject="test";
        date=Poco::Timestamp();
        
        Poco::Net::MailMessage message;
		message.setDate(date);
		message.setSender(sender);
		message.setSubject(subject);
		message.setContentType(contentType);
		message.addContent(new Poco::Net::StringPartSource(content));
		for(unsigned int i=0;i<recipients.size();i++){
			message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,recipients[i]));
		}
		for(unsigned int i=0;i<cc.size();i++){
			message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,cc[i]));
		}
		for(unsigned int i=0;i<bcc.size();i++){
			message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,bcc[i]));
		}
		for(unsigned int i=0; i < attachmentPaths.size(); i++){
			message.addAttachment(attachmentPaths[i], new Poco::Net::FilePartSource(ofToDataPath(attachmentPaths[i], true)));
		}

        
        
		session->sendMessage(message);
	}catch(Poco::Exception e){
		ofLog(OF_LOG_ERROR,"cannot connect to the server");
	}
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