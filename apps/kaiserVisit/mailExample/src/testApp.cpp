#include "testApp.h"

#include "testApp.h"

#include "Poco/Net/MailMessage.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Exception.h"

using Poco::Net::SMTPClientSession;
using Poco::Exception;
using Poco::Net::MailMessage;

#define OFX_SMTP_PORT 25

void testApp::sendMail(string subject,string content) {
        
    SMTPClientSession * session;
    
    try{
        
        session=new Poco::Net::SMTPClientSession("192.168.10.120",OFX_SMTP_PORT);
        session->login();
        
        string charset = "utf-8";
        string contentType = "text/html; charset=\"utf-8\"";
        string subject = "בדיקה איך זה ?";
        
        
        MailMessage message;
        message.setDate(Poco::Timestamp());
        message.setSender("noreply@towerofdavid.org.il");
        message.setSubject(MailMessage::encodeWord(subject,charset));
        
        //message.addContent(new Poco::Net::StringPartSource(MailMessage::encodeWord(content),"text/html"),MailMessage::ENCODING_BASE64);
        message.setContentType(contentType);
        message.setContent(content,MailMessage::ENCODING_8BIT);
        
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"roikr75@gmail.com"));
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"lofipeople@gmail.com"));
        session->sendMessage(message);
        
        if (session) {
            session->close();
            session = 0;
        }
    }catch(Poco::Exception e){
        ofLog(OF_LOG_ERROR,"cannot connect to the server");
        
        if (session) {
            session->close();
        }
    }
    
}


//--------------------------------------------------------------
void testApp::setup(){
   
    sendMail("איך זה ?","<p dir=\"rtl\"><span>שלום,</span></p><p dir=\"rtl\"><span>הצטלמתי באוהל הקיסר וילהלם השני. </span></p><p dir=\"rtl\"><span>מצ\"ב מזכרת מהביקור שלי בתערוכה \"הקיסר מגיע לירושלים\" במוזיאון מגדל דוד.</span></p><p dir=\"rtl\"><span><a href=\"http://www.towerofdavid.org.il/\">www.towerofdavid.org.il</a></span></p><p>&nbsp;</p>");
             
    ::exit(0);
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