#include "testApp.h"
#include "ofxXmlSettings.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "Poco/Net/MailMessage.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Exception.h"

using Poco::Net::SMTPClientSession;
using Poco::Exception;
using Poco::Net::MailMessage;

#define OFX_SMTP_PORT 25

#define MENU_INSET 10.0
#define MENU_SEPERATOR 10.0
#define EXTENNSION "jpg"
#define IDLE_DELAY 90000 

#define LIST_TIMEOUT 10000
#define HEARTBEAT_TIME 30000


#define MENU_WIDTH 140
#define MENU_HEIGHT 768
#define PHOTO_WIDTH 864
#define PHOTO_HEIGHT 540

enum {
    STATE_SLEEP,
    STATE_NEW_IMAGE,
    STATE_IMAGES,
    STATE_OBJECTS,
    STATE_SHARE,
    STATE_MAIL,
    STATE_FACEBOOK
};

enum {
    FB_STATE_IDLE,
    FB_STATE_LOGIN,
    FB_STATE_UPLOAD
};



//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	// hacking of ofxiPhoneAppDelegate.mm - flip frame buffer width and height
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0) {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    } else {
        ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    }
     // 
    
    
    ofRegisterURLNotification(this);
    ofEnableAlphaBlending();
   
	
//    if (Settings::getBool("reset")) {
//        ofxXmlSettings xml;
//        if (xml.loadFile("settings.xml")) {
//            Settings::setString(xml.getAttribute("settings", "host", "192.168.10.97"),"host");
//            
//            xml.pushTag("settings");
//            Settings::setInt(xml.getAttribute("http", "port", 8888),"http");
//            Settings::setInt(xml.getAttribute("sender", "port", 10000),"sender");
//            Settings::setInt(xml.getAttribute("receiver", "port", 10001),"receiver");
//            Settings::setString(xml.getAttribute("smtp", "host", "192.168.10.136"),"smtp");
//            xml.popTag();
//        }
//    } 
    
      

    doc.setup("DOMDocument.xml");
    doc.load();
    
    float scale = (float)ofGetHeight()/768;
    screenMat.scale(scale, scale, 1.0);
    screenMat.translate(0.5*(ofGetWidth()-scale*1024), 0, 0);
//    
//    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
//        mat.translate(64, 0, 0);
//        mat.scale(5.0/6.0, 5.0/6.0, 1.0);
//    }
    
    layout = doc.getSymbolItem("Layout")->createInstance("layout", screenMat);
    
    menuInterface = layout.getChild("menuInterface");
    dragInterface = layout.getChild("dragInterface");
   
    
    defaultImage = doc.getSymbolItem("DefaultImage")->createInstance("defaultImage", screenMat);
    
    selectedFrame = doc.getBitmapItem("TAMB_STROKE.png");
    
    camMat = screenMat;
    camMat.preMult(layout.getChild("photo")->mat);
    
    menuMat = screenMat;
    menuMat.preMult(layout.getChild("menu")->mat);
//    cout << menuMat << endl;
//    ofVec2f pos = menuMat.preMult(ofVec3f(0,0,0));
//    cout << pos << endl;
//    
//    ofRectangle menuRect(pos.x,pos.y,menuItem.width,menuItem.height);
    thumbs.setup(scrollCollectionPrefs(true,MENU_WIDTH,MENU_HEIGHT,MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    objects.setup(scrollCollectionPrefs(true,MENU_WIDTH,MENU_HEIGHT,MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    
//    ofDirectory dir;
//    dir.allowExt("jpg");
//    dir.listDir(ofxiPhoneGetDocumentsDirectory());
//    for (int i=0; i<dir.numFiles(); i++) {
//        thumbs.addItem(dir.getPath(i));
//    }
    
//    dir.reset();
    ofxXmlSettings xml;
    
    if (xml.loadFile("objects.xml")) {
        
        xml.pushTag("objects");
        for (int i=0;i<xml.getNumTags("object");i++) {
            ofImage img;
            img.loadImage(xml.getAttribute("object", "name", "",i));
            scales[objects.addItem(img)]=xml.getAttribute("object", "scale", 0.5,i) ; // 
            

        }
                
        
        xml.popTag();
    }
    
//    ofDirectory dir;
//    dir.allowExt("png");
//    dir.listDir("objects");
//    for (int i=0; i<dir.numFiles(); i++) {
//        ofImage img;
//        img.loadImage(dir.getPath(i));
//        objects.addItem(img);
//    }
    
    
    shareLayout = doc.getSymbolItem("ShareLayout")->createInstance("shareLayout", screenMat);
    success = doc.getSymbolItem("Success")->createInstance("success", screenMat);
    shareInterface = shareLayout.getChild("shareInterface");
    
    ofxSymbolInstance *overlay = shareLayout.getChild("overlay");
    
    shareLayout.getChildMat(overlay, shareMat);
    
    mailInterface = doc.getSymbolItem("MailInterface")->createInstance("mailInterface", screenMat);
    
//    shareMat = mat;
//    shareMat.preMult(shareLayout.getChild("overlay")->mat);
    
    
   
    state = STATE_SLEEP;
  
    imageRect = ofRectangle(0,0,PHOTO_WIDTH,PHOTO_HEIGHT);
    ofImage &overlayImage = doc.getBitmapItem("MIGDAL_OVERLAY_EN.png")->getImage();
   
    offscreen.setup(overlayImage.getWidth(), overlayImage.getHeight());
    sharePixels.allocate(overlayImage.getWidth(), overlayImage.getHeight(), OF_IMAGE_COLOR_ALPHA);
    bShare = false;
    
//    mail.setup();
//    ofxRegisterMailNotification(this);
    
    ofxRegisterFacebookNotification(this);
    actionTime = ofGetElapsedTimeMillis();    
   
    
    lang = "HE";
    bSuccess = false;
    bPostImage = false;
    fbState = FB_STATE_IDLE;
    refresh();
    
    int sscale = [[UIScreen mainScreen] scale];
    ofRectangle rect = mailInterface.getChild("EMAIL_BOX")->getBoundingBox(mailInterface.mat) ;
    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
    
    // hacking of ofxiPhoneKeyboard.mm - init and setFrame - for ios6
    keyboard = new ofxiPhoneKeyboard(rect.x/sscale,rect.y/sscale,rect.width/sscale,rect.height/sscale);
	keyboard->setVisible(false);
//	keyboard->setBgColor(255, 255, 255, 0);
	keyboard->setFontColor(255,255,255, 255);
	keyboard->setFontSize(52);
//    keyboard->setText("roikr75@gmail.com");
    [keyboard->getTextField() setKeyboardType:UIKeyboardTypeEmailAddress];
    
    activeIter = items.rend();
    bNewItem = false;
    
    defaultTexts["MAIL_SUBJECT_HE"]="הייתי שם – תערוכה במגדל דוד: \"הקיסר מגיע לירושלים\"";
    defaultTexts["MAIL_SUBJECT_EN"]="I Was There – Exhibition at the Tower of David Museum: \"The Kaiser is Coming!\"";
    defaultTexts["MAIL_SUBJECT_AR"]="لقد كنت هناك – في معرض \" القيصر يزور أورشليم-القدس\" في متحف قلعة داود.";

    /*
    defaultTexts["MAIL_BODY_HE"]="<html dir='rtl'><head><meta http-equiv='content-type' content='text/html;charset=iso-8859-8-i'></head><body>שלום,<br>הצטלמתי באוהל הקיסר וילהלם השני.<br/>מצ\"ב מזכרת מהביקור שלי בתערוכה \"הקיסר מגיע לירושלים\" במוזיאון מגדל דוד.<br><a href='http://www.towerofdavid.org.il'>www.towerofdavid.org.il</a></body></html>";
    defaultTexts["MAIL_BODY_EN"]="<html><head/><body>Shalom,<br>I was in Kaiser Wilhelm's tent!<br>Here is my photo from the Exhibition \"The Kaiser is Coming!\" at the Tower of David Museum.<br><a href=\"http://www.towerofdavid.org.il\">www.towerofdavid.org.il</a></body></html>";
    defaultTexts["MAIL_BODY_AR"]="<html dir=\"rtl\"><head/><body>مرحبا،<br>لقد كنت في خيمة القيصر ويليام!<br>وهنا صورتي من معرض \" القيصر يزور أورشليم-القدس\" في متحف قلعة داود.<br><a href=\"http://www.towerofdavid.org.il\">www.towerofdavid.org.il</a></body></html>";
    */
    
    defaultTexts["MAIL_BODY_HE"]="<p dir=\"rtl\"><span>שלום,</span></p><p dir=\"rtl\"><span>הצטלמתי באוהל הקיסר וילהלם השני. </span></p><p dir=\"rtl\"><span>מצ\"ב מזכרת מהביקור שלי בתערוכה \"הקיסר מגיע לירושלים\" במוזיאון מגדל דוד.</span></p><p dir=\"rtl\"><span><a href=\"http://www.towerofdavid.org.il/\">www.towerofdavid.org.il</a></span></p><p>&nbsp;</p>";
    
    defaultTexts["MAIL_BODY_AR"]="<p dir=\"rtl\"><span>مرحبا،</span></p><p dir=\"rtl\"><span>لقد كنت في خيمة القيصر ويليام! </span></p><p dir=\"rtl\"><span>وهنا صورتي من معرض \" القيصر يزور أورشليم-القدس\" في متحف قلعة داود.</span></p><p dir=\"rtl\"><span><a href=\"http://www.towerofdavid.org.il/\">www.towerofdavid.org.il</a></span></p><p>&nbsp;</p>";
    
    defaultTexts["MAIL_BODY_EN"]="<p dir=\"ltr\"><span>Shalom,</span></p><p dir=\"ltr\"><span>I was in Kaiser Wilhelm's tent! </span></p><p dir=\"ltr\"><span>Here is my photo from the Exhibition \"The Kaiser is Coming!\" at the Tower of David Museum.</span></p><p style=\"text-align: left;\" dir=\"rtl\"><span><a href=\"http://www.towerofdavid.org.il/\">www.towerofdavid.org.il</a></span></p><p>&nbsp;</p>";
    
    defaultTexts["FACEBOOK_HE"]="גם אני פגשתי את הקיסר";
    defaultTexts["FACEBOOK_EN"]="I met the Kaiser at the Tower of David Museum!";
    defaultTexts["FACEBOOK_AR"]="انا ٔايضا التقيت بالقيصر";
    
    
    string host = Settings::getString("host");
    int httpPort = Settings::getInt("http");
    int senderPort = Settings::getInt("sender");
    
    url = "http://"+host+":"+ofToString(httpPort);
    
    cout << url << "\tsender: " << senderPort << "\treceiver: " << Settings::getInt("receiver") << "\tsmtp: " << Settings::getString("smtp") << endl;
    
    sender.setup(host,senderPort);
    receiver.setup(Settings::getInt("receiver"));
    
    heartbeatTimer = ofGetElapsedTimeMillis();
    
    list(); // if we are already register at the server plz list (else, will list in the first heartbeat)
    
    
}

void testApp::list() {
    ofxOscMessage m;
    m.setAddress("/list");
    sender.sendMessage(m); 
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
    
    if (ofGetElapsedTimeMillis()> heartbeatTimer) {
        ofxOscMessage m;
        m.setAddress("/heartbeat");
        m.addIntArg(Settings::getInt("receiver"));
        sender.sendMessage(m);
        heartbeatTimer = ofGetElapsedTimeMillis() + HEARTBEAT_TIME;
    }
    
   
    
    // check for waiting messages
    while( receiver.hasWaitingMessages() )
    {
        
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage( &m );
        
        switch (state) {
            case STATE_SLEEP:
            case STATE_NEW_IMAGE:
            case STATE_IMAGES: 
                
                
               
                if ( m.getAddress() == "/new" || m.getAddress() == "/add") {
                    cout << m.getAddress() << "\t" << m.getArgAsString(0) << endl;
                    if (m.getAddress() == "/new" &&  state != STATE_IMAGES) {
                        state = STATE_NEW_IMAGE;
                        thumbs.deselect();
                        actionTime = ofGetElapsedTimeMillis();
                        ofLoadURLAsync(url+"/photos/"+m.getArgAsString(0)+"."+EXTENNSION);
                        refresh();
                    }
                    ofLoadURLAsync(url+"/thumbs/"+m.getArgAsString(0)+"."+EXTENNSION);
                } else if ( m.getAddress() == "/remove" ) {
                    cout << m.getAddress() << "\t" << m.getArgAsString(0) << endl;
                    int itemID = thumbs.findItemByName(m.getArgAsString(0));
                    if (itemID) {
                        if (thumbs.getIsSelected() && thumbs.getSelectedID()==itemID) {
                            image.clear();
                            
                        }
                        thumbs.removeItem(itemID);
                        if (thumbs.getNumItems() == 0){
                            state = STATE_SLEEP;
                        }
                        refresh();
                    }
                }
            

                
                break;
            default:
                break;
        }
        
    }
    
    
    
    
    
    
    switch (state) {
        case STATE_SLEEP:
        
            
            break;
        case STATE_NEW_IMAGE:
        case STATE_IMAGES:
        case STATE_OBJECTS:
            
            if (ofGetElapsedTimeMillis()>actionTime+IDLE_DELAY) {
                done(false);
            }
            
            break;
        case STATE_SHARE:
            if (ofGetElapsedTimeMillis()>actionTime+IDLE_DELAY) {
                done(bSuccess);
            }
            break;
        case STATE_MAIL:
            if (ofGetElapsedTimeMillis()>actionTime+IDLE_DELAY) {
                done(true);
            }
            break;
        
        case STATE_FACEBOOK:
            

            
            break;
            
        default:
            break;
    }
    
    
    if (bShare) {
        share();
        state=STATE_SHARE;
        bShare = false;
        refresh();
    }
    
    if (bPostImage) {
        bPostImage = false;
        
    }

    objects.update();
    thumbs.update();

    
}

/*
if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
    float tw = imageRect.width/imageRect.height*cam.getHeight()/cam.getWidth();
    cam.draw(imageRect, ofRectangle((1-tw)/2,0,tw,1));
} 
*/

void testApp::urlResponse(ofHttpResponse &response) {
    if (response.status == 200) {
        string url = response.request.url;
        vector<string> split = ofSplitString(url, "/");
        if (split.size()>1) {
            if (*(split.end()-2)=="thumbs") {
                ofImage thumb;
                thumb.loadImage(response.data);
                string name = ofSplitString(split.back(), ".").front();
                cout << "urlResponse: " << name << endl;
                int itemID = thumbs.addItem(thumb,name);
                if (state==STATE_NEW_IMAGE) {
                    thumbs.select(itemID);
                }
                refresh();
            } else {
                image.loadImage(response.data);
            }
        }
        
    } else {
        cout << response.request.url << "\t" <<  response.error << endl;
    }
    
    
}


//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
    
    
    
    switch (state) {
  
        case STATE_SLEEP:  
        case STATE_NEW_IMAGE:
        case STATE_IMAGES:

            ofPushMatrix();
            glMultMatrixf(camMat.getPtr());
            defaultImage.draw();
            if (image.isAllocated()) {
                image.draw(imageRect); 
            }
            ofPopMatrix();
            
            layout.draw();//Layer(background);
            
            ofPushMatrix();
            glMultMatrixf(menuMat.getPtr());
            thumbs.draw();
            if (thumbs.getIsSelected()) {
                
                ofRectangle rect = thumbs.getRectangle(thumbs.getSelectedID());
                ofImage &im = selectedFrame->getImage();
                im.draw(rect.x-0.5*(im.getWidth()-rect.width),rect.y-0.5*(im.getHeight()-rect.height));
            }
            
            ofPopMatrix();
                        
            
            break;
        case STATE_OBJECTS:
            
            
            ofPushMatrix();
            glMultMatrixf(camMat.getPtr());
            defaultImage.draw(); // for fast transient
            if (thumbs.getIsSelected()) {
                image.draw(imageRect);
            }
            
            
            
            for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
                if (!bNewItem || iter!=items.end()-1) {
                    iter->drag.begin();
                    ofImage &image(objects.getItem(iter->objectID).image);
                    ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
                    image.draw(0,0);
                    iter->drag.end();
                }
            }
            
            ofPopMatrix();
            
            layout.draw();
            
            ofPushMatrix();
            glMultMatrixf(menuMat.getPtr());
            objects.draw();
            ofPopMatrix();
            
           
            
            if (bNewItem && activeIter != items.rend()) {
                ofPushMatrix();
                glMultMatrixf(camMat.getPtr());
                //item &it=items.front();
                activeIter->drag.begin();
                ofImage &image(objects.getItem(activeIter->objectID).image);
                ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
                image.draw(0,0);
                activeIter->drag.end();
                ofPopMatrix();
            }
            

            break;
            
        
        case STATE_SHARE:
        
        
           
           

            shareLayout.draw();
            
            ofPushMatrix();
            glMultMatrixf(shareMat.getPtr());
            
            if( shareImage.bAllocated()) {
                shareImage.draw(0, 0);
            }
            
            ofPopMatrix();
            success.draw();
            
            break;
            
        case STATE_FACEBOOK:
            ofPushMatrix();
            glMultMatrixf(screenMat.getPtr());
            fbBackground->getImage().draw(0, 0);
            ofPopMatrix();
            break;
            
        case STATE_MAIL:
        
            mailInterface.draw();
            
            break;

            
        default:
            break;
    }
    
//    ofSetColor(0);
//    ofDrawBitmapString(ofToString(state)+" "+ofToString(fbState), 5,10);
    
}

void testApp::refresh() {
    
    menuInterface->bVisible = false;
    dragInterface->bVisible = false;
    shareInterface->bVisible = false;
    

    
    ofxSymbolInstance *language = layout.getChild("langs");
    for (vector<layer>::iterator liter=language->layers.begin(); liter!=language->layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
            if (iter->name.size()==2) {
                iter->bVisible = iter->name!=lang;
            } else {
                iter->bVisible = iter->name==lang+"_S";
            }
        }
    }
    
    language = shareLayout.getChild("langs");
    for (vector<layer>::iterator liter=language->layers.begin(); liter!=language->layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
            if (iter->name.size()==2) {
                iter->bVisible = iter->name!=lang;
            } else {
                iter->bVisible = iter->name==lang+"_S";
            }
        }
    }
    
    
    
    switch (state) {
        case STATE_SLEEP:
        case STATE_NEW_IMAGE:
        case STATE_IMAGES:
        
            menuInterface->bVisible = true;
            for (vector<layer>::iterator liter=menuInterface->layers.begin(); liter!=menuInterface->layers.end(); liter++) {
                for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
                    iter->bVisible = false;
                }
            }
            
            
            menuInterface->getChild("B_NEXT_"+lang)->bVisible = thumbs.getIsSelected();
            menuInterface->getChild("T_TAKEPIC_"+lang)->bVisible = thumbs.getNumItems()==0;
            menuInterface->getChild("T_CHOOSE_"+lang)->bVisible = thumbs.getNumItems()!=0;
            
            break;
        case STATE_OBJECTS:
            dragInterface->bVisible = true;
            for (vector<layer>::iterator liter=dragInterface->layers.begin(); liter!=dragInterface->layers.end(); liter++) {
                for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
                    iter->bVisible = false;
                }
            }
        
            dragInterface->getChild("B_NEXT2_"+lang)->bVisible = true;
            dragInterface->getChild("B_BACK_"+lang)->bVisible = true;
            dragInterface->getChild("T_DRAG_"+lang)->bVisible = true;
            break;
            
        case STATE_MAIL: {
           
            layer *ly = mailInterface.getLayer("interface");
            
            for (vector<ofxSymbolInstance>::iterator iter=ly->frames.front().instances.begin(); iter!=ly->frames.front().instances.end();iter++) {
                iter->bVisible = false;
            }
            
            
            mailInterface.getChild("T_ENTER_MAIL_"+lang)->bVisible = true;
            mailInterface.getChild("B_CANCEL_"+lang)->bVisible = true;
            mailInterface.getChild("B_SEND_"+lang)->bVisible = true;
            
            
        }    
        case STATE_SHARE:
        
        
            shareInterface->bVisible = true;
            
            for (vector<layer>::iterator liter=shareInterface->layers.begin(); liter!=shareInterface->layers.end(); liter++) {
                for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
                    iter->bVisible = false;
                }
            }
            
            for (vector<layer>::iterator liter=success.layers.begin(); liter!=success.layers.end(); liter++) {
                for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
                    iter->bVisible = false;
                }
            }
            
            success.getChild("T_SUCCESSFULLY_"+lang)->bVisible = bSuccess;
            shareInterface->getChild("B_FACEBOOK_"+lang)->bVisible = true;
            shareInterface->getChild("B_MAIL_"+lang)->bVisible = true;
            shareInterface->getChild("B_FINISH_"+lang)->bVisible = true;
                    
            break;
            
        case STATE_FACEBOOK:
            
            break;
        default:
            break;
    }
    
    
    
}

void testApp::share() {
    
//    GLint defaultFramebuffer;
//    glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &defaultFramebuffer);
    
    offscreen.begin();
    
    ofSetColor(255);
   
    ofPushMatrix();
    glMultMatrixf(shareLayout.getChild("overlay")->getChild("imageMarker")->mat.getPtr());
    
    image.draw(0,0);
    
    for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        
        iter->drag.begin();
        ofImage &image(objects.getItem(iter->objectID).image);
        ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
        image.draw(0,0);
        iter->drag.end();
        
    }
    ofPopMatrix();
    
    doc.getBitmapItem("MIGDAL_OVERLAY_"+lang+".png")->draw();
    
   
    
    glReadPixels(0, 0, sharePixels.getWidth(), sharePixels.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, sharePixels.getPixels());
    
    offscreen.end();
    
//    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    
    shareImage.setFromPixels(sharePixels);
    shareImage.setImageType(OF_IMAGE_COLOR);
    
//    shareImage.update();

}

/*
void testApp::sendMail() {
        mailStruct m;
        m.subject = "test";
        m.body = "now with attachment";
        m.toRecipients.push_back("lofipeople@gmail.com");
        ofBuffer buffer;
        ofSaveImage(shareImage.getPixelsRef(), buffer,OF_IMAGE_FORMAT_PNG);
       
//        ofBuffer buffer =ofBufferFromFile(ofxiPhoneGetDocumentsDirectory()+"kaiser.jpg");
        cout << "sendMail: " << shareImage.getPixelsRef().size() << "\t" <<  buffer.size() << endl;
        m.attachments.push_back(attachmentStruct(buffer,"image/png","kaiser"));
        mail.sendMail(m);
}
*/

void testApp::sendMail() {
    
    string attachmentPath = ofxiPhoneGetDocumentsDirectory()+"kaiser.jpg";
    shareImage.saveImage(attachmentPath,OF_IMAGE_QUALITY_HIGH); // OF_IMAGE_QUALITY_MEDIUM
 
    SMTPClientSession * session;
    
    try{
        
        session=new Poco::Net::SMTPClientSession(Settings::getString("smtp"),OFX_SMTP_PORT);
        session->login();
        
        string charset = "utf-8";
        string contentType = "text/html; charset=\"utf-8\"";
        
        Poco::Net::MailMessage message;
		message.setDate(Poco::Timestamp());
		message.setSender(Settings::getString("email"));
		message.setSubject(defaultTexts["MAIL_SUBJECT_"+lang]);
        message.addContent(new Poco::Net::StringPartSource(defaultTexts["MAIL_BODY_"+lang],contentType),MailMessage::ENCODING_8BIT);
        //message.setContentType(contentType);
        //message.setContent(defaultTexts["MAIL_BODY_"+lang],MailMessage::ENCODING_8BIT);
       
		//message.addContent(,"text/html")); // ,Poco::Net::MailMessage::ENCODING_BASE64
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,keyboard->getText()));
        keyboard->setText("");
        message.addAttachment("kaiser.jpg", new Poco::Net::FilePartSource(attachmentPath));
		session->sendMessage(message);
        bSuccess = true;
        
        if (session) {
            session->close();
            session = 0;
        }
	}catch(Poco::Exception e){
		ofLog(OF_LOG_ERROR,"cannot connect to the server");
        bSuccess = false;
        if (session) {
            session->close();
        }
	}
    
    
}

void testApp::done(bool bDelete) {
    
    
    keyboard->setVisible(false);
    state = STATE_SLEEP;
    
    image.clear();
    items.clear();
    activeIter = items.rend();
    

    if (bDelete) {
        ofxOscMessage m;
        if (thumbs.getIsSelected()) {
            
            m.setAddress("/delete");
            m.addStringArg(thumbs.getItem(thumbs.getSelectedID()).name);
            sender.sendMessage(m);
            m.clear();
            
        }
    } 
    
    thumbs.clear();
    
    list();
    
    cout << "list: " << url << endl;
    
    
    refresh();

}

void testApp::exit() {
    
//    mail.exit();
    fb.logout();
}
//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
//    cout << "touchDown" << endl;
       
    if (state==STATE_SLEEP || state == STATE_NEW_IMAGE) {
        state = STATE_IMAGES;
    }
    
    actionTime = ofGetElapsedTimeMillis();
   
    
    ofVec2f menuPos = menuMat.getInverse().preMult(ofVec3f(touch.x,touch.y,0));
    ofTouchEventArgs menuTouch(touch);
    menuTouch.x = menuPos.x;
    menuTouch.y = menuPos.y;
    
    
     vector<ofxSymbolInstance*> hits;
    
    if( layout.getChild("langs")->hitTest(layout.mat.getInverse().preMult(ofVec3f(touch.x,touch.y)),hits)) {
        if (hits.front()->name.size()==2) {
            string newLang = hits.front()->name;
            if (lang!=newLang) {
                lang = newLang;
                if (state == STATE_SHARE) {
                    bShare = true;
                }
            }
        }
    }

    
    
    switch (state) {
        
        case STATE_IMAGES: {
           
            thumbs.touchDown(menuTouch);
            
            hits.clear();
            if( layout.getChild("menuInterface")->hitTest(layout.mat.getInverse().preMult(ofVec3f(touch.x,touch.y)),hits)) {
                if (hits.front()->name=="B_NEXT_"+lang) {
                    state=STATE_OBJECTS;
                }
            }
            
            
        } break;
        case STATE_OBJECTS: {
            
            ofVec2f camPos = camMat.getInverse().preMult(ofVec3f(touch.x,touch.y));
            ofTouchEventArgs camTouch(touch);
            camTouch.x = camPos.x;
            camTouch.y = camPos.y;
            lastTouch = camTouch;
            
            
            if (activeIter!=items.rend()) {
                activeIter->drag.touchDown(camTouch); // second finger ?
            }
            else {
            
                objects.touchDown(menuTouch);
                
                if (!objects.getIsInside(ofVec2f(menuTouch.x,menuTouch.y))) {
                    
                    for (vector<item>::reverse_iterator riter=items.rbegin(); riter!=items.rend(); riter++) {
                        if (riter->drag.inside(camTouch)) {
                            activeIter = riter;
                            riter->drag.touchDown(camTouch);
                            break;
                        }
                    }
                } 
            }
                
            hits.clear();
            if( layout.getChild("dragInterface")->hitTest(layout.mat.getInverse().preMult(ofVec3f(touch.x,touch.y)),hits)) {
                if (hits.front()->name=="B_BACK_"+lang) {
                    done(false);
                }
                
                if (hits.front()->name=="B_NEXT2_"+lang) {
                    bSuccess = false;
                    bShare = true; // need to render to fbo in update before draw 

                }
            }

            
           
        }   break;
            
        case STATE_SHARE: {
            
            
            hits.clear();
            if( shareLayout.getChild("shareInterface")->hitTest(shareLayout.mat.getInverse().preMult(ofVec3f(touch.x,touch.y)),hits)) {
                if (hits.front()->name=="B_FACEBOOK_"+lang && fbState == FB_STATE_IDLE) {
                    bSuccess = false;
                    
                    bPostImage = false;
                    state = STATE_FACEBOOK;
                    actionTime = ofGetElapsedTimeMillis();
                    fbBackground = doc.getBitmapItem("FB_BG_"+lang+".png");
                    string strs[] = {"publish_actions", "user_photos"};
                    fb.login(vector<string>(strs,strs+2));
                    fbState = FB_STATE_LOGIN;
                    
                }
                
                if (hits.front()->name=="B_MAIL_"+lang) {
                    state = STATE_MAIL;
                    bSuccess = false;
                    actionTime = ofGetElapsedTimeMillis();
                    keyboard->setText("");
                    keyboard->setVisible(true);
                    keyboard->openKeyboard();
                    
//                    if (mail.getCanSendMail()) {
//                        state = STATE_MAIL;
//                        bSuccess = false;
//                        actionTime = ofGetElapsedTimeMillis();
//                        sendMail();
//                    }
                }
                
                if (hits.front()->name=="B_FINISH_"+lang) {
                    done(true);
                }
            }
            
            
            
        } break;
            
        case STATE_MAIL:
            hits.clear();
            if( mailInterface.hitLayer(mailInterface.getLayer("interface"),ofVec3f(touch.x,touch.y),hits)) {
                if (hits.front()->name=="B_SEND_"+lang) {
                    keyboard->setVisible(false);  // we can get the textfield content only after keyboard closed
                    if (!keyboard->getText().empty()) {
                        sendMail();
                    }
                    
                    state = STATE_SHARE;
                }
                
                if (hits.front()->name=="B_CANCEL_"+lang) {
                    keyboard->setVisible(false);
                    state = STATE_SHARE;
                }
            }
            
            break;
            
            
        default:
            break;
    }
    refresh();
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch) {
    
    
    ofVec2f menuPos = menuMat.getInverse().preMult(ofVec3f(touch.x,touch.y,0));
    ofTouchEventArgs menuTouch(touch);
    menuTouch.x = menuPos.x;
    menuTouch.y = menuPos.y;
    
    
    switch (state) {
        case STATE_IMAGES:
            thumbs.touchMoved(menuTouch);
            break;
        case STATE_OBJECTS: {
            objects.touchMoved(menuTouch);
            
            ofVec2f pos = camMat.getInverse().preMult(ofVec3f(touch.x,touch.y));
            ofTouchEventArgs camTouch(touch);
            camTouch.x = pos.x;
            camTouch.y = pos.y;
            
            int objectID;
                
            if (activeIter==items.rend() && objects.getIsInside(menuPos) && (objectID = objects.getID(menuPos))!=0) {
            

                float angle = (ofVec2f(camTouch.x,camTouch.y)-ofVec2f(lastTouch.x,lastTouch.y)).angle(ofVec2f(-1.0,0.0));
//                cout << angle << endl;
                
                if (abs(angle) < 45) {
                   
 //                   ofRectangle rect = objects.getRectangle(objectNum);
                    item x;
//                    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
                    ofImage &image(objects.getItem(objectID).image); 
                    ofMatrix4x4 mat(ofMatrix4x4::newTranslationMatrix(camTouch.x,camTouch.y,0));
                    mat.preMultScale(ofVec3f(scales[objectID],scales[objectID],1.0));
                    x.drag.setup(image.width,image.height,mat);
                    x.drag.touchDown(lastTouch);
                    x.drag.touchMoved(camTouch);
                    x.objectID = objectID;
//                    x.scale = (100-40)/objects.getImage(x.objectNum).getWidth();
                    x.scale = menuMat.getScale().x*objects.getRectangle(objectID).width/objects.getItem(x.objectID).image.getWidth();
                    items.push_back(x);
                    activeIter = items.rbegin();
                    bNewItem = true;
                    objects.touchUp(touch);
                } 
                
                lastTouch = camTouch;
            } else {
                if (activeIter!=items.rend()) {
                    activeIter->drag.touchMoved(camTouch);
                }
            }
            
            
        }   break;
            
        default:
            break;
    }

    
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    
    
    
    ofVec2f menuPos = menuMat.getInverse().preMult(ofVec3f(touch.x,touch.y,0));
    ofTouchEventArgs menuTouch(touch);
    menuTouch.x = menuPos.x;
    menuTouch.y = menuPos.y;
    
    bNewItem = false;
    
    switch (state) {
        case STATE_IMAGES: {
            bool bSelected =thumbs.getIsSelected();
            int selectedID = thumbs.getSelectedID();
            
            thumbs.touchUp(menuTouch);
            
            if ( thumbs.getIsSelected() && (!bSelected || selectedID!=thumbs.getSelectedID())) {
                ofLoadURLAsync(url+"/photos/"+thumbs.getItem(thumbs.getSelectedID()).name+"."+EXTENNSION);
                refresh();
                
            }
            
            
        } break;
        case STATE_OBJECTS: {
            objects.touchUp(menuTouch);
            
            ofVec2f pos = camMat.getInverse().preMult(ofVec3f(touch.x,touch.y));
            ofTouchEventArgs camTouch(touch);
            camTouch.x = pos.x;
            camTouch.y = pos.y;
            
            if (activeIter!=items.rend()) {
                activeIter->drag.touchUp(camTouch);
                if (!activeIter->drag.getIsActive()) {
                    activeIter = items.rend();
                }
            } 
            
//            if (bNewItem && !items.empty() && !items.back().drag.getIsActive()) {
//                bNewItem = false;
//            }
            
        }   break;
            
        default:
            break;
    }

    
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    
    
}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}






/*
void testApp::mailComposer(int &result) {
    cout << "mailComposer: " << result << endl;
    if (result == OFXIMAIL_SEND) {
        bSuccess = true;
       
    }
    state = STATE_SHARE;
    actionTime = ofGetElapsedTimeMillis();
   
    refresh();
   
}
*/

void testApp::mailAlert(string subject) {
    cout << "sending mail alert" << endl;
    
    SMTPClientSession * session;
    
    try{
        
        session=new Poco::Net::SMTPClientSession("192.168.10.120",OFX_SMTP_PORT);
        session->login();
        
        
        Poco::Net::MailMessage message;
        message.setDate(Poco::Timestamp());
        message.setSender("noreply@towerofdavid.org.il");
        message.setSubject(subject);
        
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"roikr75@gmail.com"));
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"lofipeople@gmail.com"));
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"yaal@roth-tevet.com"));
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


void testApp::facebookEvent(ofxFBEventArgs &args) {
    cout << "facebookEvent, action: " << args.action << ", status: " << args.status << endl << "message: " << args.message << endl;
    //    ofBuffer buffer;
    //    ofSaveImage(shareImage.getPixelsRef(), buffer,OF_IMAGE_FORMAT_PNG);
    switch (args.action) {
        case FACEBOOK_ACTION_LOGIN:
            
            if (state==STATE_FACEBOOK) {
                state = STATE_SHARE;
                
            }
            actionTime = ofGetElapsedTimeMillis();
            
            switch (args.status) {
                case FACEBOOK_SUCCEEDED:
                    if (fbState == FB_STATE_LOGIN) {
                        fb.getMe();
                    }
                    break;
                case FACEBOOK_FAILED:
                    fbState = FB_STATE_IDLE;
                    break;
                default:
                    break;
            }

            break;
        case FACEBOOK_ACTION_GET_ME:
            switch (args.status) {
                case FACEBOOK_SUCCEEDED:
                    
                    if (fbState == FB_STATE_LOGIN) {
                        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
                        NSString *fbid = [defaults objectForKey:@"fbid"];
                        
                        if (!fbid || args.message!=ofxNSStringToString(fbid)) {
                            [defaults setValue:ofxStringToNSString(args.message) forKey:@"fbid"];
                            [defaults synchronize];
                            fbState = FB_STATE_UPLOAD;
                            if (state == STATE_SHARE) {
                                bSuccess = true;
                            }
                            fb.postImage(shareImage,defaultTexts["FACEBOOK_"+lang]);
                        } else {
                             mailAlert(args.message+" is trying to login into facebook repeatedly");
                            /* open an alert with an OK button */
                            NSString *message = @"Only one Facebook upload per account is allowed. You can still send your photos by e-mail.";
                            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Oops." message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
                            [alert show];
                            [alert release];
                           
                            fbState = FB_STATE_IDLE;
                            
                        }
                        
                    }
                    break;
                case FACEBOOK_FAILED:
                    fbState = FB_STATE_IDLE;
                    break;
                default:
                    break;
            }
            
            break;
        case FACEBOOK_ACTION_POST_IMAGE:
            switch (args.status) {
                case FACEBOOK_SUCCEEDED:
                    
                    
                    break;
                case FACEBOOK_FAILED:
                    break;
                    
                default:
                    break;
            }
            
            fbState = FB_STATE_IDLE;
            
            
            
            
            break;
            
        default:
            break;
    }
    
    
    refresh();
        
}
