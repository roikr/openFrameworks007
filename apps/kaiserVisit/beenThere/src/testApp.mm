#include "testApp.h"
#include "ofxXmlSettings.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#define MENU_INSET 15.0
#define MENU_SEPERATOR 15.0
#define EXTENNSION "jpg"
#define IDLE_DELAY 20000
#define SHARE_DELAY 20000

enum {
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

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
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
            xml.popTag();
        }
            
            
         
        
    } 
    
    string host = Settings::getString("host");
    int httpPort = Settings::getInt("http");
    int senderPort = Settings::getInt("sender");
    receiverPort = Settings::getInt("receiver");
    url = "http://"+host+":"+ofToString(httpPort);
    cout << url << "\tsender: " << senderPort << "\treceiver: " << receiverPort << endl;
    
    sender.setup(host,senderPort);
    receiver.setup(receiverPort);
    
    ofxOscMessage m;
    m.setAddress("/list");
    m.addIntArg(receiverPort);
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
    defaultImage = doc.getSymbolItem("DefaultImage")->createInstance("defaultImage", screenMat);
    
    selectedFrame = doc.getBitmapItem("TAMB_STROKE.png");
    
    ofxBitmapItem *overlayItem = doc.getBitmapItem("MIGDAL_OVERLAY.png");
    
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
    shareLayout.getChildMat(shareLayout.getChild("overlay"), shareMat);
//    shareMat = mat;
//    shareMat.preMult(shareLayout.getChild("overlay")->mat);
    
    
    bTouchObject = false;
    state = STATE_IMAGES;
  
    tex.allocate(800, 600, GL_RGBA);
    fbo.setup(tex.getWidth(), tex.getHeight());
    shareImage.allocate(800, 600, OF_IMAGE_COLOR_ALPHA);
    bShare = false;
    mail.setup();
    ofxRegisterMailNotification(this);
    
    ofxRegisterFacebookNotification(this);
    actionTime = ofGetElapsedTimeMillis()-IDLE_DELAY;    
    bSetIdle = false;
    
    refresh();
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
    
    
    if (state==STATE_IMAGES) {
        // check for waiting messages
        while( receiver.hasWaitingMessages() )
        {
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage( &m );
            
            // check for mouse moved message
            bNewImage = m.getAddress() == "/new";
            if ( bNewImage || m.getAddress() == "/add") {
                if (bNewImage &&  ofGetElapsedTimeMillis()>actionTime+IDLE_DELAY) {
                    ofLoadURLAsync(url+"/photos/"+m.getArgAsString(0)+"."+EXTENNSION);
                }
                ofLoadURLAsync(url+"/thumbs/"+m.getArgAsString(0)+"."+EXTENNSION);
            } else if ( m.getAddress() == "/remove" ) {
                
                int itemID = thumbs.findItemByName(m.getArgAsString(0));
                if (itemID) {
                    if (thumbs.getIsSelected() && thumbs.getSelectedID()==itemID) {
                        image.clear();
                        
                    }
                    thumbs.removeItem(itemID);
                    refresh();
                }
                            
                
            }		
        }
    }
    
    


           
    objects.update();
    thumbs.update();
   
    
    if (bShare) {
        share();
        state=STATE_SHARE;
        bShare = false;
    }
    
    switch (state) {
        case STATE_IMAGES:
        case STATE_OBJECTS:
        case STATE_SHARE:
            if (bSetIdle && ofGetElapsedTimeMillis()>actionTime+IDLE_DELAY) {
                bSetIdle = false;
                state = STATE_IMAGES;
                thumbs.deselect();
                image.clear();
                items.clear();
                refresh();
                
            }
            break;
        case STATE_MAIL:
        case STATE_FACEBOOK:
            if (ofGetElapsedTimeMillis()>actionTime+SHARE_DELAY) {
                state = STATE_IMAGES;
                thumbs.deselect();
                image.clear();
                items.clear();
                refresh();
            }
            break;
            
        default:
            break;
    }
    
    
    if (bPostImage) {
        bPostImage = false;
        if (state == STATE_FACEBOOK) {
            fb.postImage(shareImage);
            shareLayout.getChild("label_sent")->bVisible = true;
            state = STATE_SHARE;
            actionTime = ofGetElapsedTimeMillis();
            bSetIdle = true;
        }
    }
      
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
  
          
        case STATE_IMAGES:
            defaultImage.draw();
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
                        
            if (image.isAllocated()) {
                ofPushMatrix();
                glMultMatrixf(camMat.getPtr());
                image.draw(imageRect);
                
                

                
                ofPopMatrix();
                
            }
            break;
        case STATE_OBJECTS:
            
            defaultImage.draw(); // for fast transient
            ofPushMatrix();
            glMultMatrixf(camMat.getPtr());
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
        case STATE_MAIL:
        case STATE_FACEBOOK:
           
           

            shareLayout.draw();
            
            ofPushMatrix();
            glMultMatrixf(shareMat.getPtr());
            
            if( shareImage.bAllocated()) {
                shareImage.draw(0, 0);
            }
            
            ofPopMatrix();
            
            
            break;

            
        default:
            break;
    }
    
}

void testApp::refresh() {
    layout.getChild("edit")->bVisible = false;
    layout.getChild("label_take")->bVisible = false;
    layout.getChild("label_choose")->bVisible = false;
    
    layout.getChild("share")->bVisible = false;
    layout.getChild("back")->bVisible = false;
    layout.getChild("label_drag")->bVisible = false;
    
    
    switch (state) {
        case STATE_IMAGES:
            
            layout.getChild("edit")->bVisible = thumbs.getIsSelected();
            
            layout.getChild("label_take")->bVisible = thumbs.getNumItems()==0;
            layout.getChild("label_choose")->bVisible = thumbs.getNumItems()!=0;
            
            break;
        case STATE_OBJECTS:
            layout.getChild("share")->bVisible = true;
            layout.getChild("back")->bVisible = true;
            layout.getChild("label_drag")->bVisible = true;

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
    
    doc.getBitmapItem("MIGDAL_OVERLAY.png")->draw();
    
   
    
    glReadPixels(0, 0, shareImage.getWidth(), shareImage.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, shareImage.getPixels());
    
    fbo.end();
    
//    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    
    shareImage.update();

}

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


void testApp::exit() {
    mail.exit();
    fb.logout();
}
//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    cout << "touchDown" << endl;
    
    actionTime = ofGetElapsedTimeMillis();
    bSetIdle = true;
    
    ofVec2f menuPos = menuMat.getInverse().preMult(ofVec3f(touch.x,touch.y,0));
    ofTouchEventArgs menuTouch(touch);
    menuTouch.x = menuPos.x;
    menuTouch.y = menuPos.y;
    
    
    switch (state) {
        
        case STATE_IMAGES: {
           
            thumbs.touchDown(menuTouch);
            
            vector<ofxSymbolInstance*> hits;
            if (layout.hitTest(ofVec2f(touch.x,touch.y),hits)) {
                for (vector<ofxSymbolInstance*>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                    if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="edit") {
                        state=STATE_OBJECTS;
                        break;
                    }
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
                
            vector<ofxSymbolInstance*> hits;
            if (layout.hitTest(ofVec2f(touch.x,touch.y),hits)) {
                for (vector<ofxSymbolInstance*>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                    if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="share") {
                        shareLayout.getChild("label_sent")->bVisible = false;
                        bShare = true; // need to render to fbo in update before draw 
                        break;
                    }
                    if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="back") {
                        state=STATE_IMAGES;
                        items.clear();
                        break;
                    }
                }
            }
           
        }   break;
            
        case STATE_SHARE: {
            
            vector<ofxSymbolInstance*> hits;
            if (shareLayout.hitTest(ofVec2f(touch.x,touch.y),hits)) {
                for (vector<ofxSymbolInstance*>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                    if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="mail") {
                        if (mail.getCanSendMail()) {
                            state = STATE_MAIL;
                            shareLayout.getChild("label_sent")->bVisible = false;
                            actionTime = ofGetElapsedTimeMillis();
                            bSetIdle = true;
                            sendMail();
                        }
                        
                        break;
                    }
                    if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="finish") {
                        state = STATE_IMAGES;
                        
                        items.clear();
                        image.clear();
                        
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
                        break;
                    }
                    
                    if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="facebook") {
                        shareLayout.getChild("label_sent")->bVisible = false;
                        
                        bPostImage = false;
                        state = STATE_FACEBOOK;
                        actionTime = ofGetElapsedTimeMillis();
                        
                        if (fb.getIsLoggedIn()) {
                            fb.logout();
                        }
                        string strs[] = {"publish_actions", "user_photos"};
                        fb.login(vector<string>(strs,strs+2));
                        break;
                    }
                }
            }
            
        } break;
            
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
                if (bNewImage && state==STATE_IMAGES && ofGetElapsedTimeMillis()>actionTime+IDLE_DELAY) {
                    thumbs.select(itemID);
                    bNewImage = false;
                }
                refresh();
            } else {
                image.loadImage(response.data);
            }
        }
        
    }
    
    
}

void testApp::mailComposer(int &result) {
    cout << "mailComposer: " << result << endl;
    if (result == OFXIMAIL_SEND) {
        shareLayout.getChild("label_sent")->bVisible = true;
    }
    state = STATE_SHARE;
    actionTime = ofGetElapsedTimeMillis();
    bSetIdle = true;
   
}

void testApp::facebookEvent(ofxFBEventArgs &args) {
    cout << "facebookEvent, action: " << args.action << ", status: " << args.status << endl << "message: " << args.message << endl;
    //    ofBuffer buffer;
    //    ofSaveImage(shareImage.getPixelsRef(), buffer,OF_IMAGE_FORMAT_PNG);
    switch (args.action) {
        case FACEBOOK_ACTION_LOGIN:
            switch (args.status) {
                case FACEBOOK_SUCEEDED:
                    bPostImage = true;
                    //fb.postImage(shareImage);
                    break;
                case FACEBOOK_FAILED:
                    state = STATE_SHARE;
                    actionTime = ofGetElapsedTimeMillis();
                    bSetIdle = true;
                   
                    
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
    
    
    
        
}
