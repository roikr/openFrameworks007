#include "testApp.h"

enum {
    STATE_IMAGES,
    STATE_OBJECTS
};

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    
    
    images.setup(scrollCollectionPrefs(true,ofRectangle(ofGetWidth()-100,0,100, ofGetHeight()),20,20,5,0x00FF00,100));
    objects.setup(scrollCollectionPrefs(true,ofRectangle(ofGetWidth()-100,0,100, ofGetHeight()),20,20,5,0x00FF00,100));
    
    ofDirectory dir;
    dir.allowExt("jpg");
    dir.listDir("images");
    for (int i=0; i<dir.numFiles(); i++) {
        images.addItem(dir.getPath(i));
    }
    
    dir.reset();
    dir.allowExt("png");
    dir.listDir("objects");
    for (int i=0; i<dir.numFiles(); i++) {
        objects.addItem(dir.getPath(i));
    }
    
    state = STATE_IMAGES;
    bIsDown = false;
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
   
    images.update();
    objects.update();
    
    if (state == STATE_OBJECTS && bIsDown && objects.getIsDown() && (ofGetElapsedTimef()-downTime)*1000 > 250) {
        bIsDown = false;
        ofRectangle rect = objects.getRectangle(objects.getDownNum());
        item x;
        
//        cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
        
        
        x.drag.setup(rect);
        x.drag.touchDown(downTouch);
        x.objectNum = objects.getDownNum();
        x.scale = (100-40)/objects.getImage(x.objectNum).getWidth();
        items.push_back(x);
    }

}

//--------------------------------------------------------------
void testApp::draw(){	
    ofSetHexColor(0xFFFFFF);
    
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
        image.draw(inset+(dw-w)/2, inset+(dh-h)/2, w, h);
    }

    
    switch (state) {
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
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    
        
    bIsDown = false;
    switch (state) {
        case STATE_IMAGES:
            images.touchDown(touch);
            break;
        case STATE_OBJECTS: {
            
            objects.touchDown(touch);
            if (objects.getIsInside(ofVec2f(touch.x,touch.y)) && objects.getIsDown()) {
                downTouch = touch;
                downTime = ofGetElapsedTimef();
                bIsDown = true;
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
            for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
                iter->drag.touchMoved(touch);
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
    
    if (ofRectangle(0, 0, 50, 50).inside(ofVec2f(touch.x,touch.y))) {
        
        switch (state) {
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

