#include "testApp.h"
#include "ofxXmlSettings.h"

#define MENU_INSET 20.0
#define MENU_SEPERATOR 20.0
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
	
    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        url = xml.getAttribute("settings", "url", "",0);
        xml.pushTag("settings");
        sender.setup(xml.getAttribute("sender", "host", ""),xml.getAttribute("sender", "port", 10000));
        port = xml.getAttribute("receiver", "port", 10001);
        receiver.setup(port);
        
        ofxOscMessage m;
        m.setAddress("/list");
        m.addIntArg(port);
        sender.sendMessage(m);
        
        cout << url << endl;
    }
    
	
    
        
    doc.setup("DOMDocument.xml");
    doc.load();
    
        
    ofMatrix4x4 mat;
    float scale = (float)ofGetHeight()/768;
    mat.scale(scale, scale, 1.0);
    mat.translate(0.5*(ofGetWidth()-scale*1024), 0, 0);
    
//    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
//        mat.translate(64, 0, 0);
//        mat.scale(5.0/6.0, 5.0/6.0, 1.0);
//    }
    
    layout = doc.getSymbolItem("Layout")->createInstance("layout", mat);
    
    layout.getChild("snap")->bVisible = false;
    layout.getChild("pimp")->bVisible = true;
    layout.getChild("share")->bVisible = false;
    layout.getChild("back")->bVisible = false;
    
    background = layout.getLayer("background");
    scratch = layout.getLayer("scratch");
    
    ofxBitmapItem *scratchItem = doc.getBitmapItem("SCRATCHES OVERLAY.png");
    imageRect = ofRectangle(0,0,scratchItem->getWidth(),scratchItem->getHeight());
    camMat = mat;
    camMat.preMult(layout.getChild("scratch")->mat);
    
    ofxBitmapItem *menuItem = doc.getBitmapItem("MENU_BACKGROUND.png");
    ofMatrix4x4 menuMat = mat;
    menuMat.preMult(layout.getChild("menu")->mat);
//    cout << menuMat << endl;
//    ofVec2f pos = menuMat.preMult(ofVec3f(0,0,0));
//    cout << pos << endl;
//    
//    ofRectangle menuRect(pos.x,pos.y,menuItem.width,menuItem.height);
    thumbs.setup(scrollCollectionPrefs(true,menuMat,menuItem->getWidth(),menuItem->getHeight(),MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    objects.setup(scrollCollectionPrefs(true,menuMat,menuItem->getWidth(),menuItem->getHeight(),MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    
//    ofDirectory dir;
//    dir.allowExt("jpg");
//    dir.listDir(ofxiPhoneGetDocumentsDirectory());
//    for (int i=0; i<dir.numFiles(); i++) {
//        thumbs.addItem(dir.getPath(i));
//    }
    
//    dir.reset();
    ofDirectory dir;
    dir.allowExt("png");
    dir.listDir("objects");
    for (int i=0; i<dir.numFiles(); i++) {
        ofImage img;
        img.loadImage(dir.getPath(i));
        objects.addItem(img);
    }
    
    
    shareLayout = doc.getSymbolItem("ShareLayout")->createInstance("shareLayout", mat);

    shareBackground = shareLayout.getLayer("background");
    shareScratch = shareLayout.getLayer("scratch");
    shareMat = mat;
    shareMat.preMult(shareLayout.getChild("scratch")->mat);
    
    
    bTouchObject = false;
    state = STATE_IMAGES;
  
    
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
		if ( m.getAddress() == "/add" ) {
        
            ofLoadURLAsync(url+"/thumbs/"+m.getArgAsString(0)+"_THUMB."+EXTENNSION);
            if (find(images.begin(), images.end(), m.getArgAsString(0)) == images.end()) {
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
    
    
    bool bSelected =thumbs.getIsSelected();
    int numSelected = thumbs.getSelectedNum();
    thumbs.update();
    if ( thumbs.getIsSelected() && (!bSelected || numSelected!=thumbs.getSelectedNum())) {
        if (thumbs.getSelectedNum()<images.size()) {
            ofLoadURLAsync(url+"/photos/"+images[thumbs.getSelectedNum()]+"."+EXTENNSION);
        }
    }

           
    objects.update();
   
    
    
  
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
            layout.drawLayer(background);
            thumbs.draw();
            
            if (image.isAllocated()) {
                ofPushMatrix();
                glMultMatrixf(camMat.getPtr());
                image.draw(imageRect);
                ofPopMatrix();
//                ofImage &image = thumbs.getImage(thumbs.getSelectedNum());
//                float inset = 20; 
//                float dw = ofGetWidth() - 100 - 2*inset;
//                float dh = ofGetHeight() - 2 * inset;
//                float sw = dw/image.getWidth();
//                float sh = dh/image.getHeight();
//                float scale = MIN(sh,sw);
//                float w = scale*image.getWidth();
//                float h = scale*image.getHeight();
//                rect = ofRectangle(inset+(dw-w)/2, inset+(dh-h)/2,w, h);
//                image.draw(rect);
                
            }
            ofEnableAlphaBlending();
            layout.drawLayer(scratch);
            ofDisableAlphaBlending();
            break;
        case STATE_OBJECTS:
            layout.drawLayer(background);
            if (thumbs.getIsSelected()) {
                ofPushMatrix();
                glMultMatrixf(camMat.getPtr());
                image.draw(imageRect);
                ofPopMatrix();
            }
            
            objects.draw();
            ofEnableAlphaBlending();
            for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
                iter->drag.begin();
                ofPushMatrix();
                ofScale(iter->scale, iter->scale);
                objects.getImage(iter->objectNum).draw(0,0);
                ofPopMatrix();
//                iter->drag.draw();
                iter->drag.end();
            }
            layout.drawLayer(scratch);
            ofDisableAlphaBlending();

            break;
            
        
        case STATE_SHARE:
           
            
            shareLayout.drawLayer(shareBackground);
            
            ofPushMatrix();
            glMultMatrixf(shareMat.getPtr());
            
            if (thumbs.getIsSelected()) {
                
                image.draw(imageRect);
               
            }
            
            
            ofEnableAlphaBlending();
            for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
                iter->drag.begin();
                ofPushMatrix();
                ofScale(iter->scale, iter->scale);
                objects.getImage(iter->objectNum).draw(0,0);
                ofPopMatrix();
                //                iter->drag.draw();
                iter->drag.end();
            }
            ofPopMatrix();
            shareLayout.drawLayer(shareScratch);
            ofDisableAlphaBlending();
            
            break;

            
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    
        
    vector<ofxSymbolInstance> hits = layout.hitTest(ofVec2f(touch.x,touch.y));
    switch (state) {
        
        case STATE_IMAGES: {
           
            thumbs.touchDown(touch);
            
            for (vector<ofxSymbolInstance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE && iter->name=="pimp") {
                    state=STATE_OBJECTS;
                    layout.getChild("pimp")->bVisible = false;
                    layout.getChild("share")->bVisible = true;
                    layout.getChild("back")->bVisible = true;
                    break;
                }
            }
            
        } break;
        case STATE_OBJECTS: {
            
            objects.touchDown(touch);
            if (objects.getIsInside(ofVec2f(touch.x,touch.y)) && objects.getIsDown()) {
                lastTouch = touch;
                objectNum = objects.getDownNum(); // getDownNum valid only at down stage
                bTouchObject = true;
            }
                        
            for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
                if (iter->drag.inside(touch)) {
                    iter->drag.touchDown(touch);
                    break;
                }
            }
            
            for (vector<ofxSymbolInstance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE && iter->name=="share") {
                    state=STATE_SHARE;
                    
                    
                    break;
                }
                if (iter->type==SYMBOL_INSTANCE && iter->name=="back") {
                    state=STATE_IMAGES;
                    
                    layout.getChild("pimp")->bVisible = true;
                    layout.getChild("share")->bVisible = false;
                    layout.getChild("back")->bVisible = false;
                    
                    items.clear();
                    break;
                }
            }
           
        }   break;
            
        case STATE_SHARE: {
            state = STATE_IMAGES;
            
            layout.getChild("pimp")->bVisible = true;
            layout.getChild("share")->bVisible = false;
            layout.getChild("back")->bVisible = false;
            
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
            m.addIntArg(port);
            sender.sendMessage(m);
            
            cout << "list: " << url << endl;
        } break;
            
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
   
    
    
    switch (state) {
        case STATE_IMAGES:
            thumbs.touchMoved(touch);
            break;
        case STATE_OBJECTS:
            objects.touchMoved(touch);
            
            if (bTouchObject) {
                float angle = (ofVec2f(touch.x,touch.y)-ofVec2f(lastTouch.x,lastTouch.y)).angle(ofVec2f(-1.0,0.0));
                //cout << angle << endl;
                
                if (angle == 0) {
                    bTouchObject = false;
                    ofRectangle rect = objects.getRectangle(objectNum);
                    item x;
//                    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
                    x.drag.setup(rect);
                    x.drag.touchDown(lastTouch);
                    x.drag.touchMoved(touch);
                    x.objectNum = objectNum;
//                    x.scale = (100-40)/objects.getImage(x.objectNum).getWidth();
                    x.scale = rect.width/objects.getImage(x.objectNum).getWidth();
                    items.push_back(x);
                    objects.touchUp(touch);
                } 
                
                lastTouch = touch;
            } else {
                for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
                    iter->drag.touchMoved(touch);
                }
            }
            
            
            break;
            
        default:
            break;
    }

    
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    switch (state) {
        case STATE_IMAGES:
            thumbs.touchUp(touch);
            break;
        case STATE_OBJECTS:
            objects.touchUp(touch);
            for (vector<item >::iterator iter=items.begin(); iter!=items.end(); iter++) {
                iter->drag.touchUp(touch);
            }
            
            break;
            
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

//void testApp::pictureTaken(ofImage &image) {
//    cout << "pictureTaken: " << image.getWidth() << "\t" << image.getHeight() << endl;
//    ofDirectory dir;
//    
//    dir.allowExt("jpg");
//    dir.listDir(ofxiPhoneGetDocumentsDirectory());
//    string filename = ofxiPhoneGetDocumentsDirectory()+"IMAGE_"+ofToString(dir.numFiles())+".jpg";
//    cout<<filename << endl;
//    float width = imageRect.width/imageRect.height*image.getHeight();
//    image.crop((image.width-width)/2, 0, width, image.height);
//    
//       
//    UInt8 *data = image.getPixels(); // unsigned char
//    
//    
//    NSInteger myDataLength = image.getWidth() * image.getHeight() * 3;
//    
//    for (int i = 0; i < myDataLength; i+=3)
//    {
//        UInt8 r_pixel = data[i];
//        UInt8 g_pixel = data[i+1];
//        UInt8 b_pixel = data[i+2];
//        
//        int outputRed = (r_pixel * .393) + (g_pixel *.769) + (b_pixel * .189);
//        int outputGreen = (r_pixel * .349) + (g_pixel *.686) + (b_pixel * .168);
//        int outputBlue = (r_pixel * .272) + (g_pixel *.534) + (b_pixel * .131);
//        
//        if(outputRed>255)outputRed=255;
//        if(outputGreen>255)outputGreen=255;
//        if(outputBlue>255)outputBlue=255;
//        
//        
//        data[i] = outputRed;
//        data[i+1] = outputGreen;
//        data[i+2] = outputBlue;
//    }
//    
//        
//    image.saveImage(filename);
//    thumbs.addItem(filename);
//    
//    layout.getChild("snap")->bVisible = false;
//    layout.getChild("pimp")->bVisible = true;
//    
//    
//    state = STATE_IMAGES;
//    
//}

void testApp::urlResponse(ofHttpResponse &response) {
    if (response.status == 200) {
        string url = response.request.url;
        vector<string> split = ofSplitString(url, "/");
        if (split.size()>1) {
            if (*(split.end()-2)=="thumbs") {
                ofImage thumb;
                thumb.loadImage(response.data);
                thumbs.addItem(thumb);
            } else {
                image.loadImage(response.data);
            }
        }
        
    }
}
