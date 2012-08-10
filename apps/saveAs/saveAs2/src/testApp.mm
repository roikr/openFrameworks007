#include "testApp.h"
#include "ofxXmlSettings.h"

#define STAGE_WIDTH 4096
#define STAGE_HEIGHT 2048

#define BORDER_SIZE 1024


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    
    
    doc.setup("DOMDocument.xml");
    doc.load();
    layout = doc.getSymbolItem("Spread_1")->createInstance("layout");
    
    for (vector<layer>::iterator iter=layout.layers.begin(); iter!=layout.layers.end(); iter++) {
        layers[iter->name] = distance(layout.layers.begin(), iter);
    }
    
//    layout.font.setGlobalDpi(72);
//    layout.font.loadFont("LetterGothicStd.otf", 72,true,false);  // antialiased, no full charset
//    layout.lineHeight = 1.2;
    
   
    
    
    
//    for (int i=0; i<references.bitmaps.size(); i++) {
//        zOffsets.push_back(ofRandom(-20, 20));
//    }
        
    cam.setZoom(0.25f);
	cam.setMinZoom(0.25f);
	cam.setMaxZoom(10.0f);
	cam.setScreenSize( ofGetWidth(), ofGetHeight() );
//    cout << ofGetWidth() << "," << ofGetHeight() << endl;
	cam.setViewportConstrain( ofVec3f(-(STAGE_WIDTH/2+BORDER_SIZE), -(STAGE_HEIGHT/2+BORDER_SIZE)), ofVec3f(STAGE_WIDTH/2+BORDER_SIZE, STAGE_HEIGHT/2+BORDER_SIZE)); //limit browseable area, in world units
	
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
//    ofBackgroundHex(0x303030);
    ofBackground(255);
    
    int bufferSize = 512;
	int sampleRate = 44100;
    ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 2);	
    
    
    
    
    
    layer *videoLayer = layout.getLayer("VIDEO");
    for (vector<ofxSymbolInstance>::iterator iter = videoLayer->instances.begin();iter!=videoLayer->instances.end();iter++) {
        if (iter->type==BITMAP_INSTANCE) {
            cout << iter->bitmapItem->href << endl;
            
            videoItem v;
            v.path = ofToDataPath(ofFile(iter->bitmapItem->href).getBaseName()+".mp4");
                        
            if ( ofFile(v.path).exists()) {
                ofVec2f topLeft = iter->mat.preMult(ofVec3f(0,0));
                ofVec2f bottomRight = iter->mat.preMult(ofVec3f(iter->bitmapItem->getWidth(),iter->bitmapItem->getHeight()));
                v.rect = ofRectangle(topLeft.x, topLeft.y, bottomRight.x-topLeft.x, bottomRight.y-topLeft.y);
                v.bitmapItem = iter->bitmapItem;
                v.mat = iter->mat;
                v.bVisible = false;
                cout << "video: " << v.path << endl;
                videos.push_back(v);
            }
        }
    }
    
	
    current = videos.end();
    
}



//--------------------------------------------------------------
void testApp::update(){
    
    cam.update();
    
    if (cam.getIsAnimating()) {
        updateLayout();
    }
    
    player.update();
    
    if (current!=videos.end() && !player.getIsPlaying()) {
        current = videos.end();         
    }
    
    for (vector<videoItem>::iterator iter=videos.begin(); iter!=videos.end(); iter++) {
        if (player.getIsPlaying()) {
            if (iter == current && !iter->bVisible) {
                player.stop();
                break;
            }
        } else {
            if (current==videos.end() && iter->bVisible) {
                current = iter;
                player.play(current->path);
                break;
            }
        }
    }
         
}



ofVec2f testApp::worldToScreen( ofVec2f p ){
	
    return (p-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT)+cam.offset)*cam.zoom+0.5f*ofVec2f(ofGetWidth(),ofGetHeight());
    
}

float testApp::fadeFactor(ofRectangle& rect) {
    ofVec2f p = worldToScreen(ofVec2f(rect.x,rect.y)+0.5*ofVec2f(rect.width,rect.height));
    ofVec2f q = 0.5f*ofVec2f(ofGetWidth(),ofGetHeight());
    float distFactor = ofClamp(p.distance(q)/(0.5*(rect.width*cam.zoom+ofGetWidth())), 0, 1);
    
    return 3*rect.width*cam.zoom/ofGetWidth()*(1-distFactor);
}

void testApp::updateLayout() {
    layer *imagesLayer = layout.getLayer("ref images");
    for (vector<ofxSymbolInstance>::iterator iter = imagesLayer->instances.begin();iter!=imagesLayer->instances.end();iter++) {
        if (iter->type==BITMAP_INSTANCE) {
            ofVec2f topLeft = iter->mat.preMult(ofVec3f(0,0));
            ofVec2f bottomRight = iter->mat.preMult(ofVec3f(iter->bitmapItem->getWidth(),iter->bitmapItem->getHeight()));
            ofRectangle rect = ofRectangle(topLeft.x, topLeft.y, bottomRight.x-topLeft.x, bottomRight.y-topLeft.y);
            iter->alphaMultiplier = fadeFactor(rect);
            
            
//            cout << iter->bitmapItem->name << "\t" << fade << "\t" << rect.x << "\t" << rect.y << "\t"<< rect.width << "\t"<< rect.height << "\t" <<endl;
            
        }
    } 
    
    imagesLayer = layout.getLayer("VIDEO");
    for (vector<ofxSymbolInstance>::iterator iter = imagesLayer->instances.begin();iter!=imagesLayer->instances.end();iter++) {
        if (iter->type==BITMAP_INSTANCE) {
            ofVec2f topLeft = iter->mat.preMult(ofVec3f(0,0));
            ofVec2f bottomRight = iter->mat.preMult(ofVec3f(iter->bitmapItem->getWidth(),iter->bitmapItem->getHeight()));
            ofRectangle rect = ofRectangle(topLeft.x, topLeft.y, bottomRight.x-topLeft.x, bottomRight.y-topLeft.y);
            
            if (current==videos.end() || !player.getIsFrameVisible() || current->rect != rect) {
                iter->alphaMultiplier = fadeFactor(rect);
                
            }
        }
    } 
    
    layout.update();
    
    ofVec2f offset(STAGE_WIDTH/2, STAGE_HEIGHT/2);
    ofVec2f p1 = cam.screenToWorld(ofVec2f(0,0))+offset;
    ofVec2f p2 = cam.screenToWorld(ofVec2f(ofGetWidth(),ofGetHeight()))+offset;
    
    //cout << p1.x << "\t" << p1.y << "\t" << p2.x << "\t" << p2.y << endl;
    
    for (vector<videoItem >::iterator iter=videos.begin();iter!=videos.end();iter++) {
        ofVec2f q1(iter->rect.x,iter->rect.y);
        ofVec2f q2(iter->rect.x+iter->rect.width,iter->rect.y+iter->rect.height);
        
        
        //        if (p1.x>q2.x || p2.x<q1.x || p1.y >q2.y || p2.y<q1.y) {
        if (p1.x>q2.x || p2.x<q1.x || p1.y >q2.y || p2.y<q1.y) {
            if (iter->bVisible) {
                //                cout << "leaving " << iter->path << endl;
                iter->bVisible = false;
                
            } 
        } else {
            if (!iter->bVisible) {
                //                cout << "entering " << iter->path << endl;
                iter->bVisible = true;
                
            } 
        }
        
        if (current==iter) {
            videoFade = fadeFactor(current->rect);
        }
    }

}

//--------------------------------------------------------------
void testApp::draw(){
    
    cam.apply();
    
    ofPushMatrix();
    
    ofTranslate(-STAGE_WIDTH/2, -STAGE_HEIGHT/2);
    
    
    ofPushMatrix();
    glTranslatef(0, 0, -300);
    
    layout.drawLayer(layout.getLayer("FLOOR"));
    layout.drawLayer(layout.getLayer("LINKS"));
    ofPopMatrix();
    ofPushMatrix();
    glTranslatef(0, 0, -200);
    layout.drawLayer(layout.getLayer("Tag Words"));
    ofPopMatrix();
    ofPushMatrix();
    glTranslatef(0, 0, -100);
    layout.drawLayer(layout.getLayer("TEXT"));
    ofPopMatrix();
    layout.drawLayer(layout.getLayer("Render Bodies"));    
    
    
    
    ofPushStyle();
    
    layout.drawLayer(layout.getLayer("ref images"));
    layout.drawLayer(layout.getLayer("VIDEO"));
    
    ofPopStyle();
        
    
    ofPushStyle();
    
    if (current!=videos.end()) {
        
        
      
        ofSetColor(255, 255, 255, 255*videoFade);
        
        ofPushMatrix();
        glMultMatrixf(current->mat.getPtr());
        
        player.draw(ofRectangle(0,0,current->bitmapItem->getWidth(),current->bitmapItem->getHeight()),ofRectangle(0,0,1,1)); // current->rect        
        ofPopMatrix();
        
        
        
    }
    
    ofPopStyle();
    
    
    ofPopMatrix();
    
    cam.reset();
        
//    cam.drawDebug(); //see info on ofxPanZoom status
//	
//	glColor4f(0,0,0,1);
//	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );	
//    
//    
//    
//    int counter = 0;
//    for (vector<videoItem>::iterator iter=videos.begin(); iter!=videos.end(); iter++) {
//        if (iter->bVisible) {
//            counter++;
//        }
//    }
//    
//    ofDrawBitmapString("visibleVideos: " + ofToString( counter ),  10, ofGetHeight() - 30 );
    
}

//--------------------------------------------------------------
void testApp::audioOut( float * output, int bufferSize, int nChannels ){
    
    // sanity check inside
    
    player.audioRequested(output, bufferSize, nChannels);
    
    for (int i = 0; i < nChannels*bufferSize; i++){
        
        output[i] *= videoFade;// * gain;
        
    }
    	
}


//--------------------------------------------------------------
void testApp::exit(){
    doc.release();
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    cam.touchDown(touch);
}



//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    cam.touchMoved(touch);
    
    updateLayout();
    
    
    

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    cam.touchUp(touch);

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    cam.touchDoubleTap(touch);
//    cam.setZoom(1.0f);	//reset zoom
//	cam.lookAt( ofVec3f() ); //reset position
//    videoPos = cam.screenToWorld(ofVec2f(touch.x,touch.y));
//    player.play(ofToDataPath("Cromwell and Lowburn Bridges Dive Queenstown.mp4"));
//    player.play(ofToDataPath("Israel_History_Land_and People.mov"));
   
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

