#include "testApp.h"
#include "ofxXmlSettings.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "Poco/Net/MailMessage.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"
#include "Poco/Exception.h"

using Poco::Exception;

#define OFX_SMTP_PORT 25

#define MENU_INSET 15.0
#define MENU_SEPERATOR 15.0
#define EXTENNSION "jpg"
#define IDLE_DELAY 10000
#define SHARE_DELAY 180000

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
   
	
    if (Settings::getBool("reset")) {
        ofxXmlSettings xml;
        if (xml.loadFile("settings.xml")) {
            Settings::setString(xml.getAttribute("settings", "host", "192.168.10.97"),"host");
            
            xml.pushTag("settings");
            Settings::setInt(xml.getAttribute("http", "port", 8888),"http");
            Settings::setInt(xml.getAttribute("sender", "port", 10000),"sender");
            Settings::setInt(xml.getAttribute("receiver", "port", 10001),"receiver");
            Settings::setString(xml.getAttribute("smtp", "host", "192.168.10.136"),"smtp");
            xml.popTag();
        }
    } 
    
    string host = Settings::getString("host");
    int httpPort = Settings::getInt("http");
    int senderPort = Settings::getInt("sender");
    receiverPort = Settings::getInt("receiver");
    url = "http://"+host+":"+ofToString(httpPort);
    string smtp = Settings::getString("smtp");
    cout << url << "\tsender: " << senderPort << "\treceiver: " << receiverPort << endl;
    
    sender.setup(host,senderPort);
    receiver.setup(receiverPort);
    
    ofxOscMessage m;
    m.setAddress("/list");
    m.addIntArg(receiverPort);
    sender.sendMessage(m);
    
    try{
		session=new Poco::Net::SMTPClientSession(smtp,OFX_SMTP_PORT);
        session->login();
        
	}catch(Poco::Exception e){
		ofLog(OF_LOG_ERROR,"cannot connect to the server");
	}
    
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
    
    ofxBitmapItem *overlayItem = doc.getBitmapItem("MIGDAL_OVERLAY_HE.png");
    
    imageRect = ofRectangle(0,0,overlayItem->getWidth(),overlayItem->getHeight());
    camMat = screenMat;
    camMat.preMult(layout.getChild("photo")->mat);
    
    ofxBitmapItem *menuItem = doc.getBitmapItem("MENU_BACKGROUND.png");
    menuMat = screenMat;
    menuMat.preMult(layout.getChild("menu")->mat);
//    cout << menuMat << endl;
//    ofVec2f pos = menuMat.preMult(ofVec3f(0,0,0));
//    cout << pos << endl;
//    
//    ofRectangle menuRect(pos.x,pos.y,menuItem.width,menuItem.height);
    thumbs.setup(scrollCollectionPrefs(true,menuItem->getWidth(),menuItem->getHeight(),MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    objects.setup(scrollCollectionPrefs(true,menuItem->getWidth(),menuItem->getHeight(),MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    
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
            scales.insert(scales.begin(),xml.getAttribute("object", "scale", 0.5,i) ); // 
            objects.addItem(img);

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
    shareInterface = shareLayout.getChild("shareInterface");
    shareLayout.getChildMat(shareLayout.getChild("overlay"), shareMat);
    
    mailInterface = doc.getSymbolItem("MailInterface")->createInstance("mailInterface", screenMat);
    
//    shareMat = mat;
//    shareMat.preMult(shareLayout.getChild("overlay")->mat);
    
    
    bTouchObject = false;
    state = STATE_SLEEP;
  
    tex.allocate(800, 600, GL_RGBA);
    fbo.setup(tex.getWidth(), tex.getHeight());
    shareImage.allocate(800, 600, OF_IMAGE_COLOR_ALPHA);
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
	keyboard->setFontSize(20);
    keyboard->setText("roikr75@gmail.com");
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
                    if (bNewImage &&  state == STATE_SLEEP) {
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
                iter->drag.begin();
                ofImage &image(objects.getItem(iter->objectID).image);
                ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
                image.draw(0,0);
                iter->drag.end();
            }
            ofPopMatrix();
            
            layout.draw();
            
            ofPushMatrix();
            glMultMatrixf(menuMat.getPtr());
            objects.draw();
            ofPopMatrix();
            
           
            
            if (!newItem.empty()) {
                ofPushMatrix();
                glMultMatrixf(camMat.getPtr());
                item &it=newItem.front();
                it.drag.begin();
                ofImage &image(objects.getItem(it.objectID).image);
                ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
                image.draw(0,0);
                it.drag.end();
                ofPopMatrix();
            }
            

            break;
            
        
        case STATE_SHARE:
        
        case STATE_FACEBOOK:
           
           

            shareLayout.draw();
            
            ofPushMatrix();
            glMultMatrixf(shareMat.getPtr());
            
            if( shareImage.bAllocated()) {
                shareImage.draw(0, 0);
            }
            
            ofPopMatrix();
            
            
            break;
            
        case STATE_MAIL:
        
            
            
            
            shareLayout.draw();
            
            ofPushMatrix();
            glMultMatrixf(shareMat.getPtr());
            
            if( shareImage.bAllocated()) {
                shareImage.draw(0, 0);
            }
            
            ofPopMatrix();
            
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
        case STATE_FACEBOOK:
        
            shareInterface->bVisible = true;
            
            for (vector<layer>::iterator liter=shareInterface->layers.begin(); liter!=shareInterface->layers.end(); liter++) {
                for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
                    iter->bVisible = false;
                }
            }
            
            shareInterface->getChild("T_SUCCESSFULLY_"+lang)->bVisible = bSuccess;
            shareInterface->getChild("B_FACEBOOK_"+lang)->bVisible = true;
            shareInterface->getChild("B_MAIL_"+lang)->bVisible = true;
            shareInterface->getChild("B_FINISH_"+lang)->bVisible = true;
                    
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
        
        image.draw(imageRect);
        
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
    
    try{
        Poco::Net::MailMessage message;
		message.setDate(Poco::Timestamp());
		message.setSender("post@kaiser.lofipeople.com");
		message.setSubject("been there, done that");
        message.setContentType("text/plain");
		message.addContent(new Poco::Net::StringPartSource("testing"));
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,keyboard->getText()));
        message.addAttachment("kaiser.png", new Poco::Net::FilePartSource(attachmentPath));
		session->sendMessage(message);
        bSuccess = true;
	}catch(Poco::Exception e){
		ofLog(OF_LOG_ERROR,"cannot send mail");
        bSuccess = false;
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
        m.addIntArg(receiverPort);
        sender.sendMessage(m);
        
        cout << "list: " << url << endl;
    } else {
        thumbs.deselect();
    }
    
    refresh();

}

void testApp::exit() {
    if (session) {
        session->close();
    }
//    mail.exit();
    fb.logout();
}
//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    cout << "touchDown" << endl;
    
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
            lang = hits.front()->name;
            
            if (lang=="AR") {
                lang="HE";
            
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
            
            vector<item>::reverse_iterator riter;
            for (riter=items.rbegin(); riter!=items.rend(); riter++) {
                if (riter->drag.getIsActive()) {
                    break;
                }
            }
            
            bool bDragActive = riter!=items.rend();
            ofVec2f camPos = camMat.getInverse().preMult(ofVec3f(touch.x,touch.y));
            ofTouchEventArgs camTouch(touch);
            camTouch.x = camPos.x;
            camTouch.y = camPos.y;
            
            if (bDragActive) {
                riter->drag.touchDown(camTouch);
            } else {
            
                objects.touchDown(menuTouch);
                
                if (objects.getIsInside(ofVec2f(menuTouch.x,menuTouch.y)) && objects.getIsDown()) {
                    lastTouch = camTouch;
                    objectID = objects.getDownID(); // getDownNum valid only at down stage
                    bTouchObject = true;
                } else {
                    for (riter=items.rbegin(); riter!=items.rend(); riter++) {
                        if (riter->drag.inside(camTouch)) {
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
                    if (!keyboard->getText().empty()) {
                        sendMail();
                    }
                    keyboard->setVisible(false);
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

            
            if (bTouchObject) {
                float angle = (ofVec2f(camTouch.x,camTouch.y)-ofVec2f(lastTouch.x,lastTouch.y)).angle(ofVec2f(-1.0,0.0));
                //cout << angle << endl;
                
                if (angle == 0) {
                    bTouchObject = false;
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
                    newItem.push_back(x);
                    objects.touchUp(touch);
                } 
                
                lastTouch = camTouch;
            } else {
                if (newItem.empty()) {
                    for (vector<item>::reverse_iterator iter=items.rbegin(); iter!=items.rend(); iter++) {
                        iter->drag.touchMoved(camTouch);
                        
                    }
                } else{
                    newItem.front().drag.touchMoved(camTouch);
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
            
            if (newItem.empty()) {
                for (vector<item >::reverse_iterator iter=items.rbegin(); iter!=items.rend(); iter++) {
                    iter->drag.touchUp(camTouch);
                }
            } else {
                newItem.front().drag.touchUp(camTouch);
                items.push_back(newItem.front());
                newItem.clear();
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
                if (state==STATE_NEW_IMAGE && !thumbs.getIsSelected()) {
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
