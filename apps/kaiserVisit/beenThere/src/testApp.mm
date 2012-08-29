#include "testApp.h"
#include "ofxXmlSettings.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#define MENU_INSET 15.0
#define MENU_SEPERATOR 15.0
#define EXTENNSION "jpg"

enum {
    STATE_IMAGES,
    STATE_OBJECTS,
    STATE_SHARE
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
    refresh();
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
    
    
    
    // check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
		// check for mouse moved message
        bNewImage = m.getAddress() == "/new";
		if ( bNewImage || m.getAddress() == "/add") {
            if (bNewImage) {
                ofLoadURLAsync(url+"/photos/"+m.getArgAsString(0)+"."+EXTENNSION);
            }
            ofLoadURLAsync(url+"/thumbs/"+m.getArgAsString(0)+"_THUMB."+EXTENNSION);
            if (find(images.begin(), images.end(), m.getArgAsString(0)) == images.end()) {
                cout << "will add: " << m.getArgAsString(0) << endl;
                images.push_back(m.getArgAsString(0));
            } 
            
            cout << images.back() << endl;
            
		} else if ( m.getAddress() == "/remove" ) {
            
            vector<string>::iterator iter = find(images.begin(),images.end(), m.getArgAsString(0));
            if (iter!=images.end()) {
                cout << "remove: " << m.getArgAsString(0) << endl;
                int pos = distance(images.begin(),iter);
                images.erase(iter);
                thumbs.removeItem(pos);
                
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
    
    if (ofGetElapsedTimeMillis()>idleTimer) {
        state = STATE_IMAGES;
        thumbs.deselect();
        refresh();
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
                
                ofRectangle rect = thumbs.getRectangle(images.size()-thumbs.getSelectedNum()-1);
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
                ofImage &image(objects.getImage(iter->objectNum));
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
                ofImage &image(objects.getImage(it.objectNum));
                ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
                image.draw(0,0);
                it.drag.end();
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
            
            layout.getChild("label_take")->bVisible = images.empty();
            layout.getChild("label_choose")->bVisible = !images.empty();
            
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
        ofImage &image(objects.getImage(iter->objectNum));
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
    if (mail.getCanSendMail()) {
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
}


void testApp::exit() {
    mail.exit();
}
//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    
    idleTimer = ofGetElapsedTimeMillis()+60000;
    
    ofVec2f menuPos = menuMat.getInverse().preMult(ofVec3f(touch.x,touch.y,0));
    ofTouchEventArgs menuTouch(touch);
    menuTouch.x = menuPos.x;
    menuTouch.y = menuPos.y;
    
    
    switch (state) {
        
        case STATE_IMAGES: {
           
            thumbs.touchDown(menuTouch);
            
            vector<ofxSymbolInstance*> hits = layout.hitTest(ofVec2f(touch.x,touch.y));
            for (vector<ofxSymbolInstance*>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="edit") {
                    state=STATE_OBJECTS;
                    break;
                }
            }
            
        } break;
        case STATE_OBJECTS: {
            
            objects.touchDown(menuTouch);
            
            ofVec2f camPos = camMat.getInverse().preMult(ofVec3f(touch.x,touch.y));
            ofTouchEventArgs camTouch(touch);
            camTouch.x = camPos.x;
            camTouch.y = camPos.y;
            
            
            if (objects.getIsInside(ofVec2f(menuTouch.x,menuTouch.y)) && objects.getIsDown()) {
                lastTouch = camTouch;
                objectNum = objects.getDownNum(); // getDownNum valid only at down stage
                bTouchObject = true;
            }
                        
            for (vector<item>::reverse_iterator iter=items.rbegin(); iter!=items.rend(); iter++) {
                if (iter->drag.inside(camTouch)) {
                    iter->drag.touchDown(camTouch);
                    break;
                }
            }
            
            vector<ofxSymbolInstance*> hits = layout.hitTest(ofVec2f(touch.x,touch.y));
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
           
        }   break;
            
        case STATE_SHARE: {
            
            vector<ofxSymbolInstance*> hits = shareLayout.hitTest(ofVec2f(touch.x,touch.y));
            for (vector<ofxSymbolInstance*>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="mail") {
                    shareLayout.getChild("label_sent")->bVisible = false;
                    sendMail();
                    break;
                }
                if ((*iter)->type==SYMBOL_INSTANCE && (*iter)->name=="finish") {
                    state = STATE_IMAGES;
                    
                    
                    
                    items.clear();
                    thumbs.clear();
                    
                    
                    
                    ofxOscMessage m;
                    
                    
                    if (!images.empty()) {
                        m.setAddress("/delete");
                        m.addStringArg(images[0]);
                        sender.sendMessage(m);
                        images.clear();
                        m.clear();
                        image.clear();
                    }
                    
                    m.setAddress("/list");
                    m.addIntArg(receiverPort);
                    sender.sendMessage(m);
                    
                    cout << "list: " << url << endl;
                    break;
                }
            }
            
            
        } break;
            
        default:
            break;
    }
    refresh();
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
   
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
                    ofImage &image = objects.getImage(objectNum);
                    ofMatrix4x4 mat(ofMatrix4x4::newTranslationMatrix(camTouch.x,camTouch.y,0));
                    mat.preMultScale(ofVec3f(scales[objectNum],scales[objectNum],1.0));
                    x.drag.setup(image.width,image.height,mat);
                    x.drag.touchDown(lastTouch);
                    x.drag.touchMoved(camTouch);
                    x.objectNum = objectNum;
//                    x.scale = (100-40)/objects.getImage(x.objectNum).getWidth();
                    x.scale = menuMat.getScale().x*objects.getRectangle(objectNum).width/objects.getImage(x.objectNum).getWidth();
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
            int numSelected = thumbs.getSelectedNum();
            
            thumbs.touchUp(menuTouch);
            
            if ( thumbs.getIsSelected() && (!bSelected || numSelected!=thumbs.getSelectedNum())) {
                if (thumbs.getSelectedNum()<images.size()) {
                    ofLoadURLAsync(url+"/photos/"+images[thumbs.getSelectedNum()]+"."+EXTENNSION);
                    refresh();
                }
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



void testApp::urlResponse(ofHttpResponse &response) {
    if (response.status == 200) {
        string url = response.request.url;
        vector<string> split = ofSplitString(url, "/");
        if (split.size()>1) {
            if (*(split.end()-2)=="thumbs") {
                ofImage thumb;
                thumb.loadImage(response.data);
                thumbs.addItem(thumb);
                if (bNewImage) {
                    thumbs.select(0);
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
}
