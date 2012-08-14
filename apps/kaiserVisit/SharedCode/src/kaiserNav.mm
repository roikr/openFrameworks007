//
//  kaiserNav.cpp
//  kaiserNav
//
//  Created by Roee Kremer on 7/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "kaiserNav.h"
#include "ofxXmlSettings.h"

enum {
    STATE_IDLE,
    STATE_TUTORIAL,
    STATE_NAVIGATION
};


void kaiserNav::updateOverlays() {
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
//        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height)));
        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(image.getWidth(),image.getHeight())));
//        iter->second.second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(image.getWidth(),image.getHeight())));
        
        
    }
    
    
    
    ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
    string titleName = imageName+"_C_"+lang;
    string subTitleName = imageName+"_SC_"+lang;
    
    for (vector<layer>::iterator liter=titles->layers.begin(); liter!=titles->layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->instances.begin(); iter!=liter->instances.end();iter++) {
            if (iter->type==SYMBOL_INSTANCE) {
                iter->bVisible = iter->name == titleName || (bSubTitle && iter->name == subTitleName);
            }
        }
    }
    
    
    
//    ofVec2f camOffset = cam.offset*cam.zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
        
    if (bCaptionActive) {
        
        
        
        ofxSymbolInstance *child = imageLayout.getChild(caption.name);
        ofMatrix4x4 mat; // should initialized to general transform if any exist
        mat.preMult(child->mat); // we set the caption name to the correspond marker when we create it
        
        ofVec2f trans(cam.worldToScreen(mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(image.getWidth(),image.getHeight())));
        
        screenMarker.mat.makeTranslationMatrix(trans);
        
        floating.setAnchor(trans); 
        
        ofRectangle rect = caption.getBoundingBox();
        
//        cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;

        caption.mat.makeTranslationMatrix(floating.getPos()-0.5*ofVec2f(rect.width,rect.height));
        caption.alphaMultiplier = floating.getFade();
//        caption.update();
        if (floating.getFade()<=0) {
            bCaptionActive = false;
        }
        
        
    }
    
    
    layer *langLayer = interfaceLayout.getLayer("language");
    for (vector<ofxSymbolInstance>::iterator iter=langLayer->instances.begin(); iter!=langLayer->instances.end(); iter++) {
        if (iter->type==SYMBOL_INSTANCE) {
            iter->alphaMultiplier = lang == iter->name ? 1.0 : 0.6;
        }
    }
//    interfaceLayout.update();
}

void kaiserNav::setCaption(string name) {
    bCaptionActive = true;
    
    captionName = name;
    
    screenMarker = doc.getSymbolItem("MARKER_SCREEN")->createInstance(name);
    
    caption = doc.getSymbolItem(captionName+'_'+lang)->createInstance(name);
    ofRectangle rect = caption.getBoundingBox();
    
    ofxSymbolInstance *child = imageLayout.getChild(caption.name);
    ofMatrix4x4 mat; // should initialized to general transform if any exist
    mat.preMult(child->mat); // we set the caption name to the correspond marker when we create it
    
    floating.setup(ofRectangle(35, 35, ofGetWidth()-70, 535), rect, 150,cam.worldToScreen(mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(image.getWidth(),image.getHeight())));
    
}

void kaiserNav::setup(){	
    
    
	ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        xml.pushTag("settings");
        for (int i=0;i<xml.getNumTags("camera");i++) {
            dragScale p;
            p.name = xml.getAttribute("camera", "name", "",i);
            
            p.minZoom = xml.getAttribute("camera", "minZoom", 0.1,i);
            p.zoomOut = xml.getAttribute("camera", "zoomOut", 0.2,i);
            p.maxZoom = xml.getAttribute("camera", "maxZoom", 1.5,i);
            p.zoomIn = xml.getAttribute("camera", "zoomIn", 1.0,i);
            
            p.zoom = xml.getAttribute("camera", "zoom", 1.0,i);
            p.offset = ofVec2f(xml.getAttribute("camera", "x", 0.0,i),xml.getAttribute("camera", "y", 0.0,i));
            settings.push_back(p);
        }
    }
    
    
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
   
    
    interfaceLayout = doc.getSymbolItem("Layout")->createInstance("layout");
    interfaceLayout.getChildMat(interfaceLayout.getChild("idle")->getChild("video"), videoMat);    
   
    screenOverlay = doc.getSymbolItem("SCREEN_OVERLY")->createInstance("screen");
    
	lang = "HE";
    setImage("I1");	
    
    
	
	ofSetCircleResolution(32);

    setState(STATE_IDLE);
    timer = ofGetElapsedTimeMillis();
    
    ofBackground(0);
    
        
}


void kaiserNav::setImage(string name) {
    
    
    this->imageName = name;
    image.loadImage(name+".png", 1024);
    
    if (!image.getDidLoad()) {
        cout << "setImage: could not find " << name << endl;
        return;
    }
    
    
    
    
    imageLayout = doc.getSymbolItem(name)->createInstance(name);
    
    layer *markersLayer = imageLayout.getLayer("markers");
    
    markers.clear();
    for (vector<ofxSymbolInstance>::iterator iter=markersLayer->instances.begin(); iter!=markersLayer->instances.end();iter++) {
        if (iter->type==SYMBOL_INSTANCE) {
            ofxSymbolInstance marker = doc.getSymbolItem("MARKER_IMAGE")->createInstance(iter->name,ofMatrix4x4());
            markers.push_back(make_pair(&*iter,marker));
            cout << iter->name << endl;
        }
    }
    
    

    
    layer *selected = interfaceLayout.getLayer("selected");
    string selectedName = imageName+"_OV";
    
   
    for (vector<ofxSymbolInstance>::iterator iter=selected->instances.begin(); iter!=selected->instances.end();iter++) {
        if (iter->type==SYMBOL_INSTANCE) {
            iter->bVisible = iter->name == selectedName;
        }
    }
    
    
    ofVec2f p1 = videoMat.preMult(ofVec3f());
    ofVec2f p2 = videoMat.preMult(ofVec3f(960,540));
    ofVec2f size = p2-p1;
    ofRectangle window(p1.x,p1.y,size.x, size.y);
    

    
    
    
    vector<dragScale>::iterator iter;
    for (iter=settings.begin(); iter!=settings.end(); iter++) {
        if (iter->name == name) {
            break;
        }
    }
            
    if (iter!=settings.end()) {
        ofMatrix4x4 mat;
        mat.makeScaleMatrix(iter->zoom,iter->zoom,1.0f);
        mat.translate(iter->offset);
        cam.setup(window, image.getWidth(), image.getHeight(),mat);
        cam.setMinZoom(iter->minZoom);
        cam.setMaxZoom(iter->maxZoom);
        
    } else {
    
        cam.setup(window, image.getWidth(), image.getHeight());
        cam.setMinZoom(960.0f/(float)image.getWidth());
        cam.setMaxZoom(1.0f);
    }
    
    	
    bCaptionActive = false;
    bSubTitle = false;
   
}

void kaiserNav::setState(int state) {
    
    interfaceLayout.getChild("idle")->bVisible = false;
    interfaceLayout.getChild("tutorial")->bVisible = false;

    this->state = state;
    
    ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
    titles->bVisible = state!=STATE_IDLE;
    
    switch (this->state) {
        case STATE_IDLE:
            bCaptionActive = false;
            interfaceLayout.getChild("idle")->bVisible = true;
            break;
            
        case STATE_TUTORIAL:
            interfaceLayout.getChild("tutorial")->bVisible = true;
            break;
            
        default:
            break;
    }
}


void kaiserNav::update() {
    cam.update();
    
    if (cam.getIsAnimating()) {
        updateOverlays();
    }

#ifdef TARGET_OF_IPHONE
    player.update();

    
    if (state!=STATE_IDLE && ofGetElapsedTimeMillis()>timer) {
        setState(STATE_IDLE);
    }
    
    if (state==STATE_IDLE && !player.getIsPlaying()) {
        player.play(ofToDataPath("video.mov"));
    }
    
    if (state!=STATE_IDLE && player.getIsPlaying()) {
        player.stop();
    }
#endif
}


void kaiserNav::draw2nd() {
    ofPushMatrix();
    
    switch (state) {
        case STATE_NAVIGATION:
        case STATE_TUTORIAL:
            
            cam.begin(); //put all our drawing under the ofxPanZoom effect
            ofTranslate(ofVec2f(-0.5*ofVec2f(image.getWidth(),image.getHeight())));
            image.draw();
            cam.end();	//back to normal ofSetupScreen() projection
            break;
            
        case STATE_IDLE:
#ifdef TARGET_OF_IPHONE
            ofPushMatrix();
            glMultMatrixf(videoMat.getPtr());
            player.draw(ofRectangle(0,0,player.getWidth(),player.getHeight()));
            ofPopMatrix();
#endif           
            break;
        default:
            break;
    }
    
    
    

    
    ofEnableAlphaBlending();
    screenOverlay.draw();
    
    if (bCaptionActive) {
        screenMarker.draw();
        floating.draw();
        caption.draw();
        
    }
    
    
    ofPopMatrix();
    
}

void kaiserNav::draw() {
    //    deep.begin();
    //    deep.draw();
    //    deep.end();
    
    ofPushMatrix();
    
	cam.begin(); //put all our drawing under the ofxPanZoom effect
    ofPushMatrix();
    
    ofVec2f imagePos(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
    ofTranslate(imagePos);

    ofDisableAlphaBlending();
    image.draw();
    ofEnableAlphaBlending();
    
    imageLayout.draw();
    
    
    ofPopMatrix();
    
//    //draw space constrains	
//	float limitX = imagePos.x;
//    float limitY = imagePos.y;
//    int s = 25;
//    glColor4f(1, 0, 0, 1);
//    ofRect(-limitX , -limitY , 2 * limitX, s);
//    ofRect(limitX - s , -limitY , s, 2 * limitY);
//    ofRect(-limitX , limitY - s , s, -2 * limitY);	
//    ofRect(limitX , limitY - s, -2 * limitX, s);		
//    glColor4f(1, 1, 1, 1);
	
	cam.end();	//back to normal ofSetupScreen() projection
	
    if (state==STATE_NAVIGATION) {
        for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
            iter->second.draw();
        }
    }
    
    
    
    if (bCaptionActive) {
        floating.draw();
        caption.draw();
    }

#ifdef TARGET_OF_IPHONE
    if (state==STATE_IDLE) {
//        float offset = 0.5*(ofGetWidth()-player.getWidth());
//        player.draw(ofRectangle(offset,offset,player.getWidth(),player.getHeight()));
        ofPushMatrix();
        glMultMatrixf(videoMat.getPtr());
        player.draw(ofRectangle(0,0,player.getWidth(),player.getHeight()));
        ofPopMatrix();
    }
#endif
    
    interfaceLayout.draw();
    
    ofPopMatrix();
    ofPushStyle();
	cam.draw(); //see info on ofxPanZoom status
    ofPopStyle();
}

void kaiserNav::exit() {
}

void kaiserNav::touchDown(ofTouchEventArgs &touch){
    
    timer = ofGetElapsedTimeMillis()+30000;
    
    switch (state) {
        case STATE_IDLE:
            
            setState(STATE_TUTORIAL);
            break;
        case STATE_TUTORIAL:
            setState(STATE_NAVIGATION);
            break;
        case STATE_NAVIGATION: {
            cam.touchDown(touch); //fw event to cam
            
            //	ofVec2f p =  cam.screenToWorld( ofVec3f( touch.x, touch.y) ) + 0.5*ofVec2f(width,height);	//convert touch (in screen units) to world units
            
            ofVec2f p(touch.x,touch.y);
            
            //    cout << p.x << "\t" << p.y << endl;
            
            if (bCaptionActive && !caption.hitTest(ofVec2f(touch.x,touch.y)).empty()) {
                bCaptionActive =false;
            }
            
            for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
                
                if (!iter->second.hitTest(ofVec2f(touch.x,touch.y)).empty()) {
                    cout << iter->first->name << endl;
                    setCaption(iter->first->name);
                }
            }
            
//            ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
//            titles->bVisible = false;
            
            vector<ofxSymbolInstance> hits = interfaceLayout.hitLayer(interfaceLayout.getLayer("thumbs"),ofVec2f(touch.x,touch.y));
            for (vector<ofxSymbolInstance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE) {
                    cout << iter->name << endl;
                    setImage(iter->name);
                }
            }
            
            hits = interfaceLayout.hitLayer(interfaceLayout.getLayer("language"),ofVec2f(touch.x,touch.y));
            for (vector<ofxSymbolInstance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE) {
                    cout << iter->name << endl;
                    lang = iter->name;
                    if (bCaptionActive) {
                        setCaption(captionName);
                    }
                }
            }
            
            
            hits = interfaceLayout.getChild("titles")->hitTest(interfaceLayout.mat.getInverse().preMult(ofVec3f(touch.x,touch.y)));
            for (vector<ofxSymbolInstance>::iterator iter=hits.begin(); iter!=hits.end(); iter++) {
                if (iter->type==SYMBOL_INSTANCE) {
                    if (iter->name == imageName+"_C_"+lang) {
                        bSubTitle = !bSubTitle;
                    }
                    
                }
            }

            
            

        } break;
        default:
            break;
    }
    
     updateOverlays();
}


void kaiserNav::touchMoved(ofTouchEventArgs &touch){

	cam.touchMoved(touch); //fw event to cam
    updateOverlays();
}


void kaiserNav::touchUp(ofTouchEventArgs &touch){
	cam.touchUp(touch);	//fw event to cam
}


void kaiserNav::touchDoubleTap(ofTouchEventArgs &touch){
//	cam.touchDoubleTap(touch); //fw event to cam
//	cam.setZoom(1.0f);	//reset zoom
//	cam.lookAt( ofVec3f() ); //reset position
    ofVec2f pos(touch.x,touch.y);
    if (cam.inside(pos)) {
        vector<dragScale>::iterator iter;
        for (iter=settings.begin(); iter!=settings.end(); iter++) {
            if (iter->name == imageName) {
                float scl = cam.getScale();
                if (iter->zoomIn/scl > scl/iter->zoomOut) {
                    cam.animateScale(pos, iter->zoomIn);
                } else {
                    cam.animateScale(pos, iter->zoomOut);
                }
                break;
            }
        }
    }
    
}


