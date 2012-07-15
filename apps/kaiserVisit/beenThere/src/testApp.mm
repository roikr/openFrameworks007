#include "testApp.h"

enum {
    STATE_CAMERA,
    STATE_IMAGES,
    STATE_OBJECTS
};

ofColor transformColor(ofColor &src) {
    
    static ofMatrix4x4 colorMat(0.393,0.349,0.272,0.0,0.769,0.686,0.534,0.0,0.189,0.168,0.131,0.0,0.0,0.0,0.0,1.0);
    ofVec4f dst = colorMat*ofVec4f(src.r,src.g,src.b,1.0);
    
    return ofColor(dst.x,dst.y,dst.z);
}
    

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    ofxRegisterStillCameraNotification(this);
    
    images.setup(scrollCollectionPrefs(true,ofRectangle(ofGetWidth()-100,0,100, ofGetHeight()),20,20,5,0x00FF00,100));
    objects.setup(scrollCollectionPrefs(true,ofRectangle(ofGetWidth()-100,0,100, ofGetHeight()),20,20,5,0x00FF00,100));
    
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
    
    state = STATE_CAMERA;
    bTouchObject = false;
    
    scratch.loadImage("SCRATCHES_OVERLAY.png");
    
    doc.setup("DOMDocument.xml");
    
    camera.itemID = doc.itemsMap["Start"];
    camera.mat.makeIdentityMatrix();
    
    cout << ofxNSStringToString([[UIDevice currentDevice] model]) << endl;
    cout << iPhoneGetDeviceType() << endl;
    
    
    
    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
        camera.mat.translate(64, 0, 0);
        camera.mat.scale(5.0/6.0, 5.0/6.0, 1.0);
    }
    
    doc.load();
    
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
    
    if (images.getIsSelected()) {
        ofImage &image = images.getImage(images.getSelectedNum());
        float inset = 20; 
        float dw = ofGetWidth() - 100 - 2*inset;
        float dh = ofGetHeight() - 2 * inset;
        float sw = dw/image.getWidth();
        float sh = dh/image.getHeight();
        float scale = MIN(sh,sw);
        float w = scale*image.getWidth();
        float h = scale*image.getHeight();
        rect = ofRectangle(inset+(dw-w)/2, inset+(dh-h)/2,w, h);
        image.draw(rect);
        
    }

    
    switch (state) {
        case STATE_CAMERA: {
            doc.symbolItems[camera.itemID].draw(camera);
            if (cam.getIsPlaying() && cam.getIsFrameVisible()) {
                float width = ofGetWidth(); 
                float height = width/cam.getWidth()*cam.getHeight();
                cam.draw(ofRectangle(0,0.5*(ofGetHeight()-height),width,height), ofRectangle(0,0,1,1));
            } 
        } break;
        case STATE_IMAGES:
            images.draw();
            break;
        case STATE_OBJECTS:
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
            ofDisableAlphaBlending();

            break;

            
        default:
            break;
    }
    
    if (images.getIsSelected()) {
        ofEnableAlphaBlending();
        scratch.draw(rect);
        ofDisableAlphaBlending();
        
    }
    
    ofSetHexColor(0x000000);
    
    ofDrawBitmapString("tap me twice", 0, 10);

	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    
        
    
    switch (state) {
        case STATE_CAMERA: {
            vector<instance> items = doc.symbolItems[camera.itemID].hitTest(camera,ofVec2f(touch.x,touch.y));
            
            for (vector<instance>::iterator iter=items.begin(); iter!=items.end(); iter++) {
                cout << iter->itemID << "\t";
                if (iter->type == SYMBOL_INSTANCE) {
                    cout << iter->name << "\t";
                }
            }
            
            cout << endl;

        } break;
        case STATE_IMAGES:
           
            images.touchDown(touch);
            break;
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
                    x.scale = (100-40)/objects.getImage(x.objectNum).getWidth();
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
    
    if (ofRectangle(0, 0, 100, 20).inside(ofVec2f(touch.x,touch.y))) {
        
        switch (state) {
            case STATE_CAMERA:
                cam.snap();
                break;
            case STATE_IMAGES:
                state=STATE_OBJECTS;
                break;
            case STATE_OBJECTS:
                state=STATE_IMAGES;
                items.clear();
                break;
                
            default:
                break;
        }    
    }
    

    //images.touchDoubleTap(touch);
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
    image.saveImage(filename);
    images.addItem(filename);
    state = STATE_IMAGES;
    cam.stop();
}
