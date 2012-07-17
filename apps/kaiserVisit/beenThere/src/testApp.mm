#include "testApp.h"

#define MENU_INSET 20.0
#define MENU_SEPERATOR 20.0


enum {
    STATE_CAMERA,
    STATE_IMAGES,
    STATE_OBJECTS
};

ofColor transformColor(ofColor &src) {
    
//    static ofMatrix4x4 colorMat(0.393,0.349,0.272,0.0,0.769,0.686,0.534,0.0,0.189,0.168,0.131,0.0,0.0,0.0,0.0,1.0);
    static ofMatrix4x4 colorMat(0.393,0.769,0.189,0.0,0.349,0.686,0.168,0.0,0.272,0.534,0.131,0.0,0.0,0.0,0.0,1.0);
    ofVec4f dst = colorMat.postMult(ofVec4f(src.r,src.g,src.b,255.0));
    
    return ofColor(dst.x,dst.y,dst.z);
}
    

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    ofxRegisterStillCameraNotification(this);
    
        
    
    
   
    
    doc.setup("DOMDocument.xml");
    
    layout.itemID = doc.itemsMap["Layout"];
    
    
    cout << ofxNSStringToString([[UIDevice currentDevice] model]) << endl;
    cout << iPhoneGetDeviceType() << endl;
    
    
    
    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
        layout.mat.translate(64, 0, 0);
        layout.mat.scale(5.0/6.0, 5.0/6.0, 1.0);
    }
    
    doc.load();
    
    layoutSymbol = doc.symbolItems[layout.itemID]; // roikr - not so good, for non existing symbols...
    
    
    layoutSymbol.getInstance("pimp")->bVisible = false;
    layoutSymbol.getInstance("share")->bVisible = false;
    layoutSymbol.getInstance("back")->bVisible = false;
    
    background = layoutSymbol.getLayer("background");
    scratch = layoutSymbol.getLayer("scratch");
    
    bitmapItem &scratchItem = doc.bitmapItems[doc.itemsMap["SCRATCHES OVERLAY.png"]];
    imageRect = ofRectangle(0,0,scratchItem.width,scratchItem.height);
    camMat = layout.mat;
    camMat.preMult(layoutSymbol.getInstance("scratch")->mat);
    
    bitmapItem &menuItem = doc.bitmapItems[doc.itemsMap["MENU_BACKGROUND.png"]];
    ofMatrix4x4 menuMat = layout.mat;
    menuMat.preMult(layoutSymbol.getInstance("menu")->mat);
//    cout << menuMat << endl;
//    ofVec2f pos = menuMat.preMult(ofVec3f(0,0,0));
//    cout << pos << endl;
//    
//    ofRectangle menuRect(pos.x,pos.y,menuItem.width,menuItem.height);
    images.setup(scrollCollectionPrefs(true,menuMat,menuItem.width,menuItem.height,MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    objects.setup(scrollCollectionPrefs(true,menuMat,menuItem.width,menuItem.height,MENU_SEPERATOR,MENU_INSET,5,0x00FF00,100));
    
    ofDirectory dir;
    dir.allowExt("jpg");
    dir.listDir(ofxiPhoneGetDocumentsDirectory());
    for (int i=0; i<dir.numFiles(); i++) {
        images.addItem(dir.getPath(i));
    }
    
    dir.reset();
    dir.allowExt("png");
    dir.listDir("objects");
    for (int i=0; i<dir.numFiles(); i++) {
        objects.addItem(dir.getPath(i));
    }

    
    
    
    bTouchObject = false;
    state = STATE_CAMERA;
    cam.preview();
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
   
    images.update();
    objects.update();
    cam.update();
    
    
  
}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
    
    ofRectangle rect;
    

    
    
    
    switch (state) {
        case STATE_CAMERA: {
            layoutSymbol.drawLayer(layout, *background);
            ofPushMatrix();
            glMultMatrixf(camMat.getPtr());
            if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
                float tw = imageRect.width/imageRect.height*cam.getHeight()/cam.getWidth();
                cam.draw(imageRect, ofRectangle((1-tw)/2,0,tw,1));
            } 
            ofPopMatrix();
        } break;
        case STATE_IMAGES:
            layoutSymbol.drawLayer(layout, *background);
            images.draw();
            
            if (images.getIsSelected()) {
                ofPushMatrix();
                glMultMatrixf(camMat.getPtr());
                images.getImage(images.getSelectedNum()).draw(imageRect);
                ofPopMatrix();
//                ofImage &image = images.getImage(images.getSelectedNum());
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
            layoutSymbol.drawLayer(layout, *scratch);
            ofDisableAlphaBlending();
            break;
        case STATE_OBJECTS:
            layoutSymbol.drawLayer(layout, *background);
            if (images.getIsSelected()) {
                ofPushMatrix();
                glMultMatrixf(camMat.getPtr());
                images.getImage(images.getSelectedNum()).draw(imageRect);
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
            layoutSymbol.drawLayer(layout, *scratch);
            ofDisableAlphaBlending();

            break;

            
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    
        
    vector<instance> hits = layoutSymbol.hitTest(layout,ofVec2f(touch.x,touch.y));
    switch (state) {
        case STATE_CAMERA: {
            
            for (vector<instance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE && iter->name=="snap") {
                    cam.snap();
                    break;
                }
            }
                
            
//            for (vector<instance>::iterator iter=items.begin(); iter!=items.end(); iter++) {
//                cout << iter->itemID << "\t";
//                if (iter->type == SYMBOL_INSTANCE) {
//                    cout << iter->name << "\t";
//                }
//            }
//            
//            cout << endl;

        } break;
        case STATE_IMAGES: {
           
            images.touchDown(touch);
            
            for (vector<instance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE && iter->name=="pimp") {
                    state=STATE_OBJECTS;
                    layoutSymbol.getInstance("pimp")->bVisible = false;
                    layoutSymbol.getInstance("share")->bVisible = true;
                    layoutSymbol.getInstance("back")->bVisible = true;
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
            
            for (vector<instance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE && iter->name=="share") {
                    state=STATE_CAMERA;
                    
                    layoutSymbol.getInstance("snap")->bVisible = true;
                    layoutSymbol.getInstance("share")->bVisible = false;
                    layoutSymbol.getInstance("back")->bVisible = false;
                    
                    items.clear();
                    cam.preview();
                    break;
                }
                if (iter->type==SYMBOL_INSTANCE && iter->name=="back") {
                    state=STATE_IMAGES;
                    
                    layoutSymbol.getInstance("pimp")->bVisible = true;
                    layoutSymbol.getInstance("share")->bVisible = false;
                    layoutSymbol.getInstance("back")->bVisible = false;
                    
                    items.clear();
                    break;
                }
            }
           
        }   break;
            
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
   
    
    
    switch (state) {
        case STATE_IMAGES:
            images.touchMoved(touch);
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
            images.touchUp(touch);
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

void testApp::pictureTaken(ofImage &image) {
    cout << "pictureTaken: " << image.getWidth() << "\t" << image.getHeight() << endl;
    ofDirectory dir;
    
    dir.allowExt("jpg");
    dir.listDir(ofxiPhoneGetDocumentsDirectory());
    string filename = ofxiPhoneGetDocumentsDirectory()+"IMAGE_"+ofToString(dir.numFiles())+".jpg";
    cout<<filename << endl;
    float width = imageRect.width/imageRect.height*image.getHeight();
    image.crop((image.width-width)/2, 0, width, image.height);
    ofPixels &pixels = image.getPixelsRef();
    for (int i=0;i<pixels.getHeight();i++) {
        for (int j=0;j<pixels.getWidth();j++) {
            ofColor src = pixels.getColor(j, i);
            pixels.setColor(j, i, transformColor(src));
        }
    }
    
    image.saveImage(filename);
    images.addItem(filename);
    
    layoutSymbol.getInstance("snap")->bVisible = false;
    layoutSymbol.getInstance("pimp")->bVisible = true;
    
    
    state = STATE_IMAGES;
    cam.stop();
}
