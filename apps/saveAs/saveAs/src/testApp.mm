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
    layout.setup("Spread_1.xml",&doc);
    
    for (vector<layer>::iterator iter=layout.layers.begin(); iter!=layout.layers.end(); iter++) {
        layers[iter->name] = distance(layout.layers.begin(), iter);
    }
    
    layout.font.setGlobalDpi(72);
    layout.font.loadFont("LetterGothicStd.otf", 72,true,false);  // antialiased, no full charset
    layout.lineHeight = 1.2;
    
   
    
    doc.load();
    
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
    
    for (vector<layer>::iterator liter=layout.layers.begin();liter!=layout.layers.end();liter++) {
        for (vector<shape>::iterator siter=liter->shapes.begin(); siter!=liter->shapes.end(); siter++) {
            
            if (!siter->line.empty() && !siter->bitmapFill.empty()) {
                
                videoItem v;
                bitmap &bi = siter->bitmapFill.front();
                bitmapItem &item = doc.bitmaps[bi.bitmapItemID];
                
                v.path = ofToDataPath(ofFile(item.href).getBaseName()+".mp4");
                            
                if ( ofFile(v.path).exists()) {
                    v.rect = bi.rect;
                    v.translation = bi.translation;
                    v.sx = bi.sx;
                    v.sy = bi.sy;
                    v.width = item.width;
                    v.height = item.height;
                    v.bVisible = false;
                    cout << "video: " << v.path << endl;
                    videos.push_back(v);
                }
                
                
            }
        }
    }
	
    current = videos.end();
    
}



//--------------------------------------------------------------
void testApp::update(){
    cam.update();
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

//--------------------------------------------------------------
void testApp::draw(){
    
    cam.apply();
    
    ofPushMatrix();
    
    ofTranslate(-STAGE_WIDTH/2, -STAGE_HEIGHT/2);
    
    
    

    //layout.draw(ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
    
    ofPushMatrix();
    glTranslatef(0, 0, -300);
    layout.drawLayer(layout.layers[layers["FLOOR"]],ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
    layout.drawLayer(layout.layers[layers["LINKS"]],ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
    ofPopMatrix();
    ofPushMatrix();
    glTranslatef(0, 0, -200);
    layout.drawLayer(layout.layers[layers["Tag Words"]],ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
    ofPopMatrix();
    ofPushMatrix();
    glTranslatef(0, 0, -100);
    layout.drawLayer(layout.layers[layers["RENDER_TEXT"]],ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
    ofPopMatrix();
        
   layout.drawLayer(layout.layers[layers["Render Bodies"]],ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
    
    
    
    ofPushStyle();
    ofVec2f center =  ofVec2f(STAGE_WIDTH/2,STAGE_HEIGHT/2)-cam.offset;
    
    {
        layer &ly = layout.layers[layers["ref images"]];
        
        for (vector<shape>::iterator siter=ly.shapes.begin(); siter!=ly.shapes.end(); siter++) {
            if (!siter->bitmapFill.empty()) {
                bitmap &bm = siter->bitmapFill.front();
//                float zoomFactor = ofMap(cam.zoom,0.8,2,0,1,true);
//                float distFactor = ofMap((ofVec2f(bm.rect.x,bm.rect.y)+0.5*ofVec2f(bm.rect.width,bm.rect.height)).distance(center), 200, 400, 1, 0,true);
//                ofSetColor(255, 255, 255, 255*ofMap(zoomFactor*distFactor, 0, 1, 0.05, 1));
                ofSetColor(255, 255, 255, 255*fadeFactor(bm.rect));
                layout.drawBitmap(bm, ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
                
            }

        }
    }
    
    {
        layer &ly = layout.layers[layers["VIDEOS"]];
        
        for (vector<shape>::iterator siter=ly.shapes.begin(); siter!=ly.shapes.end(); siter++) {
            if (!siter->bitmapFill.empty()) {
                bitmap &bm = siter->bitmapFill.front();
                if (current==videos.end() || !player.getIsFrameVisible() || current->rect != bm.rect) {
                    ofSetColor(255, 255, 255, 255*fadeFactor(bm.rect));
                    layout.drawBitmap(bm, ofVec2f(cam.offset)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT),cam.zoom);
                }
                
            }
            
        }
    }
    
    
    
    
    ofPopStyle();
        
    
    ofPushStyle();
    
    if (current!=videos.end()) {
        
        ofRectangle &rect = current->rect;
        ofVec2f q = (ofVec2f(rect.x+rect.width,rect.y+rect.height)-0.5*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT)+cam.offset)*cam.zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
        ofVec2f p = ofVec2f(ofGetHeight(),ofGetWidth())-ofVec2f(q.y,q.x);
        
        glEnable(GL_SCISSOR_TEST);
        glScissor(p.x, p.y, rect.height*cam.zoom , rect.width*cam.zoom);
      
        ofSetColor(255, 255, 255, 255*videoFade);
        
        ofPushMatrix();
        ofTranslate(current->translation);
        ofScale(current->sx, current->sy);
        player.draw(ofRectangle(0,0,current->width,current->height),ofRectangle(0,0,1,1)); // current->rect        
        ofPopMatrix();
        
        glDisable(GL_SCISSOR_TEST);
        
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

