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

#define OFX_SMTP_PORT 25

#define MENU_INSET 10.0
#define MENU_SEPERATOR 10.0
#define EXTENNSION "jpg"
#define IDLE_DELAY 90000
#define SHARE_DELAY 180000

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



//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    
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
    
      
    string host = Settings::getString("host");
    int httpPort = Settings::getInt("http");
    int senderPort = Settings::getInt("sender");
    
    url = "http://"+host+":"+ofToString(httpPort);
    
    cout << url << "\tsender: " << senderPort << "\treceiver: " << Settings::getInt("receiver") << "\tsmtp: " << Settings::getString("smtp") << endl;
    
    sender.setup(host,senderPort);
    receiver.setup(Settings::getInt("receiver"));
    
    ofxOscMessage m;
    m.setAddress("/list");
    m.addIntArg(Settings::getInt("receiver"));
    sender.sendMessage(m);
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
    tex.allocate(overlayImage.getWidth(), overlayImage.getHeight(), GL_RGBA);
    fbo.setup(tex.getWidth(), tex.getHeight());
    shareImage.allocate(overlayImage.getWidth(), overlayImage.getHeight(), OF_IMAGE_COLOR_ALPHA);
    bShare = false;
    
//    mail.setup();
//    ofxRegisterMailNotification(this);
    
    ofxRegisterFacebookNotification(this);
    actionTime = ofGetElapsedTimeMillis();    
   
    
    lang = "HE";
    bSuccess = false;
    bPostImage = false;
    refresh();
    
    int sscale = [[UIScreen mainScreen] scale];
    ofRectangle rect = mailInterface.getChild("EMAIL_BOX")->getBoundingBox(mailInterface.mat) ;
    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
    
    keyboard = new ofxiPhoneKeyboard(rect.x/sscale,rect.y/sscale,rect.width/sscale,rect.height/sscale);
	keyboard->setVisible(false);
//	keyboard->setBgColor(255, 255, 255, 0);
	keyboard->setFontColor(255,255,255, 255);
	keyboard->setFontSize(52);
    keyboard->setText("roikr75@gmail.com");
    
    activeIter = items.rend();
    bNewItem = false;
    
    defaultTexts["MAIL_SUBJECT_HE"]="הייתי שם – תערוכת ביקור הקיסר במגדל דוד";
    defaultTexts["MAIL_SUBJECT_EN"]="I Was There – the Kaiser Exhibition at the Tower of David Museum";
    defaultTexts["MAIL_BODY_HE"]="<html><head/><body>שלום,<br>מצורפת התמונה שיצרת במסגרת התערוכה ביקור הקיסר במגדל דוד<br>כל טוב.<br/>שלך,<br>הקיסר וילהלם השני<br><a href=\"http://www.towerofdavid.org.il\">www.towerofdavid.org.il</a></body></html>";
    defaultTexts["MAIL_BODY_EN"]="<html><head/><body>Shalom,<br>Here is your photo from the Kaiser Exhibition at the Tower of David Museum<br>Best wishes,<br>Kaiser Wilhelm II</body></html>";
    defaultTexts["FACEBOOK_HE"]="גם אני פגשתי את הקיסר at Tower of David Museum";
    defaultTexts["FACEBOOK_EN"]="I met the Kaiser at the Tower of David Museum!";
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
    
    
    
    switch (state) {
        case STATE_SLEEP:
        case STATE_NEW_IMAGE:
        case STATE_IMAGES:
            // check for waiting messages
            while( receiver.hasWaitingMessages() )
            {
                
                // get the next message
                ofxOscMessage m;
                receiver.getNextMessage( &m );
                
                
                
                // check for mouse moved message
                bool bNewImage = m.getAddress() == "/new";
                if ( bNewImage || m.getAddress() == "/add") {
                    cout << m.getAddress() << "\t" << m.getArgAsString(0) << endl;
                    if (bNewImage &&  state != STATE_IMAGES) {
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
            }
            break;
        default:
            break;
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
            if (ofGetElapsedTimeMillis()>actionTime+SHARE_DELAY) {
                done(true);
            }
            break;
        
        case STATE_FACEBOOK:
            
            if (ofGetElapsedTimeMillis()>actionTime+SHARE_DELAY) {
                done(true);
            }
            
            if (bPostImage) {
                bPostImage = false;
                fb.postImage(shareImage);
                bSuccess = true;
                refresh();
                state = STATE_SHARE;
                actionTime = ofGetElapsedTimeMillis();
                
            } 
            
                
            
            
            
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
            
           
            
            if (bNewItem) {
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
    
    ofSetColor(0);
    ofDrawBitmapString(ofToString(state), 5,10);
    
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
    
    fbo.begin(tex.getTextureData().textureID);
    
    ofSetColor(255);
   
    
    if (thumbs.getIsSelected()) {
        
        image.draw(shareLayout.getChild("overlay")->getChild("imageMarker")->mat.getTranslation());
        
    }
    
    for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        
        iter->drag.begin();
        ofImage &image(objects.getItem(iter->objectID).image);
        ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
        image.draw(0,0);
        iter->drag.end();
        
    }
    
    doc.getBitmapItem("MIGDAL_OVERLAY_"+lang+".png")->draw();
    
   
    
    glReadPixels(0, 0, shareImage.getWidth(), shareImage.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, shareImage.getPixels());
    
    fbo.end();
    
//    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    
    shareImage.update();

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
    
    string attachmentPath = ofxiPhoneGetDocumentsDirectory()+"kaiser.png";
    shareImage.saveImage(attachmentPath);
    SMTPClientSession * session;
    
    try{
        
        session=new Poco::Net::SMTPClientSession(Settings::getString("smtp"),OFX_SMTP_PORT);
        session->login();
            
        
        Poco::Net::MailMessage message;
		message.setDate(Poco::Timestamp());
		message.setSender("post@kaiser.lofipeople.com");
		message.setSubject(defaultTexts["MAIL_SUBJECT_"+lang]);
       
		message.addContent(new Poco::Net::StringPartSource(defaultTexts["MAIL_BODY_"+lang],"text/html")); // ,Poco::Net::MailMessage::ENCODING_BASE64
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,keyboard->getText()));
        keyboard->setText("");
        message.addAttachment("kaiser.png", new Poco::Net::FilePartSource(attachmentPath));
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
    

    if (bDelete) {
        ofxOscMessage m;
        if (thumbs.getIsSelected()) {
            
            m.setAddress("/delete");
            m.addStringArg(thumbs.getItem(thumbs.getSelectedID()).name);
            sender.sendMessage(m);
            m.clear();
            
        }
        
        thumbs.clear();
        
        
        m.setAddress("/list");
        m.addIntArg(Settings::getInt("receiver"));
        sender.sendMessage(m);
        
        cout << "list: " << url << endl;
    } else {
        thumbs.deselect();
    }
    
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
                
                if (lang=="AR") {
                    lang="HE";
                
                }
                
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
                    state=STATE_IMAGES;
                    items.clear();
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
                if (hits.front()->name=="B_FACEBOOK_"+lang) {
                    bSuccess = false;
                    
                    bPostImage = false;
                    state = STATE_FACEBOOK;
                    actionTime = ofGetElapsedTimeMillis();
                    fbBackground = doc.getBitmapItem("FB_BG_"+lang+".png");
                    
                    if (fb.getIsLoggedIn()) {
                        fb.logout();
                    }
                    string strs[] = {"publish_actions", "user_photos"};
                    fb.login(vector<string>(strs,strs+2));
                    
                }
                
                if (hits.front()->name=="B_MAIL_"+lang) {
                    state = STATE_MAIL;
                    bSuccess = false;
                    actionTime = ofGetElapsedTimeMillis();
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
                    keyboard->setVisible(false);
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
                
            if (!bNewItem && objects.getIsInside(menuPos) && (objectID = objects.getID(menuPos))!=0) {
            

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
            
            if (bNewItem && !items.empty() && !items.back().drag.getIsActive()) {
                bNewItem = false;
            }
            
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

//--------------------------------------------------------------
void testApp::lostFocus(){
    
}

//--------------------------------------------------------------
void testApp::gotFocus(){
    
}

void testApp::launchedWithURL(string url) {
        
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

void testApp::facebookEvent(ofxFBEventArgs &args) {
    cout << "facebookEvent, action: " << args.action << ", status: " << args.status << endl << "message: " << args.message << endl;
    //    ofBuffer buffer;
    //    ofSaveImage(shareImage.getPixelsRef(), buffer,OF_IMAGE_FORMAT_PNG);
    switch (args.action) {
        case FACEBOOK_ACTION_LOGIN:
            switch (args.status) {
                case FACEBOOK_SUCEEDED:
                    if (state == STATE_FACEBOOK) {
                        bPostImage = true;
                    }
                    
                    //fb.postImage(shareImage);
                    break;
                case FACEBOOK_FAILED:
                    state = STATE_SHARE;
                    actionTime = ofGetElapsedTimeMillis();
                    
                   
                    
                    break;
                    
                default:
                    break;
            }

            break;
        case FACEBOOK_ACTION_POST_IMAGE:
            switch (args.status) {
                case FACEBOOK_SUCEEDED:
                    
                    break;
                case FACEBOOK_FAILED:
                    break;
                    
                default:
                    break;
            }
            
            
            
            
            break;
            
        default:
            break;
    }
    
    
    refresh();
        
}
