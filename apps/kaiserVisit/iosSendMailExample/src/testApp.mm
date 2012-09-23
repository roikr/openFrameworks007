#include "testApp.h"

#include "Poco/Net/MailMessage.h"

#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"

#include "Poco/Exception.h"


using Poco::Exception;

#define OFX_SMTP_HOST "192.168.10.136"
#define OFX_SMTP_PORT 25


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(127,127,127);
    
    
    
    try{
		session=new Poco::Net::SMTPClientSession(OFX_SMTP_HOST,OFX_SMTP_PORT);
//        session->login(Poco::Net::SMTPClientSession::AUTH_LOGIN,"username","passwd");
        session->login();
        
	}catch(Poco::Exception e){
		ofLog(OF_LOG_ERROR,"cannot connect to the server");
	}

    keyboard = new ofxiPhoneKeyboard(0,52,320,32);
	keyboard->setVisible(false);
	keyboard->setBgColor(255, 255, 255, 255);
	keyboard->setFontColor(0,0,0, 255);
	keyboard->setFontSize(26);
    keyboard->openKeyboard();

    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::exit(){

}

void testApp::sendmail(string subject) {
    try{
		
        Poco::Timestamp date;
       
        vector<string> recipients;
        vector<string> cc;
        vector<string> bcc;
        vector<string> attachmentPaths;
        
        recipients.push_back("roikr75@gmail.com");
        
        date=Poco::Timestamp();
        
        
        Poco::Net::MailMessage message;
		message.setDate(date);
		message.setSender("post@kaiser.lofipeople.com");
		message.setSubject(subject);
		
        message.setContentType("text/plain");
		message.addContent(new Poco::Net::StringPartSource("testing"));
        attachmentPaths.push_back(ofToDataPath("CROWN.png"));
        
 
        
		for(unsigned int i=0;i<recipients.size();i++){
			message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,recipients[i]));
		}
		for(unsigned int i=0;i<cc.size();i++){
			message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::CC_RECIPIENT,cc[i]));
		}
		for(unsigned int i=0;i<bcc.size();i++){
			message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::BCC_RECIPIENT,bcc[i]));
		}
		for(unsigned int i=0; i < attachmentPaths.size(); i++){
			message.addAttachment(attachmentPaths[i], new Poco::Net::FilePartSource(ofToDataPath(attachmentPaths[i], true)));
		}
        
        
        
		session->sendMessage(message);
	}catch(Poco::Exception e){
		ofLog(OF_LOG_ERROR,"cannot send mail");
	}

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    keyboard->setVisible(true);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    sendmail(keyboard->getText());
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

