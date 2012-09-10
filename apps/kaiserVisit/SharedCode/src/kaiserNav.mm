//
//  kaiserNav.cpp
//  kaiserNav
//
//  Created by Roee Kremer on 7/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "kaiserNav.h"
#include "ofxXmlSettings.h"

#define EXTERNAL_ZOOM 1.29032258064516
#define EXTERNAL_ZOOM_RECIP 0.775
#define WINDOW_WIDTH 992.0
#define WINDOW_HEIGHT 620.0
#define SCREEN_WIDTH 1280.0
#define SCREEN_HEIGHT 800.0

enum {
    STATE_IDLE,
    STATE_TUTORIAL,
    STATE_NAVIGATION
};


void kaiserNav::updateOverlays() {
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
//        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height)));
        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(images[imageNum].getWidth(),images[imageNum].getHeight())));
//        iter->second.second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(images[imageNum].getWidth(),images[imageNum].getHeight())));
        
        
    }
    
    
    
    
    string subTitleName = imageName+"_SC_"+lang;
    string titleName = imageName+"_C_"+lang;
    
    ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
    bool bHasSubtitle = false;
    for (vector<layer>::iterator liter=titles->layers.begin(); liter!=titles->layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
            iter->bVisible = false;
            if (iter->name == subTitleName) {
                iter->bVisible = bSubTitle;
                bHasSubtitle = true;
            }
        }
    }
    titles->getChild(titleName)->bVisible = true;
    titles->getChild("TITLE_STRIP")->bVisible = true;
    if (bHasSubtitle) {
        titles->getChild("MARKER_PLUS_"+lang)->bVisible = !bSubTitle;
        titles->getChild("MARKER_MINUSE_"+lang)->bVisible = bSubTitle;
    }
    
    for (vector<layer>::iterator liter=screenLayout.layers.begin(); liter!=screenLayout.layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
            iter->bVisible = false;
            if (iter->name == subTitleName) {
                iter->bVisible = bSubTitle;
            }
        }
    }
    
    screenLayout.getChild(titleName)->bVisible = true;
    screenLayout.getChild("TITLE_STRIP")->bVisible = true;
    
//    ofVec2f camOffset = cam.offset*cam.zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
    
    extMat = ofMatrix4x4::newTranslationMatrix(0.5*ofVec2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    extMat.preMult(cam.getTransform().getPtr());
    extMat.preMult(ofMatrix4x4::newScaleMatrix(EXTERNAL_ZOOM, EXTERNAL_ZOOM, 1.0));
    
    
    if (bCaptionActive) {
        
        ofMatrix4x4 mat; 
        imageLayout.getChildMat(imageLayout.getChild(caption.name), mat); // we set the caption name to the correspond marker when we create it
        
        ofVec3f trans(mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(images[imageNum].getWidth(),images[imageNum].getHeight()));
        floating.setAnchor(cam.worldToScreen(trans)); 
        
        ofRectangle rect = caption.getBoundingBox();
        caption.mat.makeTranslationMatrix(floating.getPos()-0.5*ofVec2f(rect.width,rect.height));
        caption.alphaMultiplier =  floating.getFade();
        
        if (floating.getFade()<=0) {
            bCaptionActive = false;
        }
        
        
        ofVec3f extAnchor = extMat.preMult(trans);
        extMarker.mat.makeTranslationMatrix(extAnchor);
        ofVec3f extPos = 0.5*ofVec2f(SCREEN_WIDTH, SCREEN_HEIGHT)+floating.getVec()*EXTERNAL_ZOOM;
        extCaption.mat.makeTranslationMatrix(extPos-0.5*ofVec2f(rect.width,rect.height));
        extCaption.alphaMultiplier = floating.getFade();

        ofVec2f vec = extAnchor-extPos;
        float length = vec.length();
        
        vec.normalize();
        float va = vec.angle(ofVec2f(1,0))*PI/180;
        float captionLength = MIN(rect.width/(2.0*abs(cos(va))),rect.height/(2.0*abs(sin(va))));
        
        
        linep1 = extPos+vec*(captionLength+20);
        linep2 = extPos+vec*(length-13.5-20);
        
        
    }
    
    ofxSymbolInstance *language = interfaceLayout.getChild("LANGUAGE");
    for (vector<layer>::iterator liter=language->layers.begin(); liter!=language->layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->frames.front().instances.begin(); iter!=liter->frames.front().instances.end();iter++) {
            if (iter->name.size()==2) {
                iter->bVisible = iter->name!=lang;
            } else {
                iter->bVisible = iter->name==lang+"_S";
            }
        }
    }
    
    
    
    
//    interfaceLayout.update();
}

void kaiserNav::setCaption(string name) {
    bCaptionActive = true;
    bSubTitle = false;
    captionName = name;
    
    extMarker = doc.getSymbolItem("MARKER_SCREEN")->createInstance(name);
    
    caption = doc.getSymbolItem(captionName+'_'+lang)->createInstance(name);
    extCaption = doc.getSymbolItem(captionName+'_'+lang)->createInstance(name);
    ofRectangle rect = caption.getBoundingBox();
    
    ofxSymbolInstance *child = imageLayout.getChild(caption.name);
    ofMatrix4x4 mat; // should initialized to general transform if any exist
    mat.preMult(child->mat); // we set the caption name to the correspond marker when we create it
    
    floating.setup(ofRectangle(24, 54, 978, 570), rect, 150,cam.worldToScreen(mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(images[imageNum].getWidth(),images[imageNum].getHeight())));
    
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
    
    xml.clear();
    if (xml.loadFile("images.xml")) {
        xml.pushTag("images");
        for (int i=0; i<xml.getNumTags("image"); i++) {
//            imagesData[xml.getAttribute("image", "name", "",i)] = make_pair(xml.getAttribute("image", "width", 0,i), xml.getAttribute("image", "height", 0,i));
            string name = xml.getAttribute("image", "name", "",i);
            imagesMap[name] = images.size();
            ofxBigImage image;
            images.push_back(image);
            images.back().setup(name, 1024,xml.getAttribute("image", "width", 0,i),xml.getAttribute("image", "height", 0,i));
            
        }
    }
    
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
   
    
    interfaceLayout = doc.getSymbolItem("Layout")->createInstance("layout");
    interfaceLayout.getChildMat(interfaceLayout.getChild("idle")->getChild("video"), videoMat); 
    
    screenLayout = doc.getSymbolItem("ScreenLayout")->createInstance("screenLayout");
    
    
	lang = "HE";
    imageName = imagesMap.begin()->first;
    imageNum = imagesMap.begin()->second;
    
    
    
    
	
	ofSetCircleResolution(32);

    setState(STATE_IDLE);
    timer = ofGetElapsedTimeMillis();
    
    ofBackground(0);
    
        
}


void kaiserNav::setImage(string name) {
    
    map<string,int>::iterator miter = imagesMap.find(name);
    if (miter==imagesMap.end()) {
        return;
    }
    
    imageName = name;
    
    if (images[imageNum].getDidLoad()) {
        images[imageNum].unload();
    }
    
    imageNum = miter->second;    
    images[imageNum].load();
    
    
    
    imageLayout = doc.getSymbolItem(name)->createInstance(name);
    
    layer *markersLayer = imageLayout.getLayer("markers");
    
    markers.clear();
    for (vector<ofxSymbolInstance>::iterator iter=markersLayer->frames.front().instances.begin(); iter!=markersLayer->frames.front().instances.end();iter++) {
        if (iter->type==SYMBOL_INSTANCE) {
            ofxSymbolInstance marker = doc.getSymbolItem("MARKER_IMAGE")->createInstance(iter->name,ofMatrix4x4());
            markers.push_back(make_pair(&*iter,marker));
            cout << iter->name << endl;
        }
    }
    
    

    
    layer *selected = interfaceLayout.getLayer("selected");
    string selectedName = imageName+"_OV";
    
   
    for (vector<ofxSymbolInstance>::iterator iter=selected->frames.front().instances.begin(); iter!=selected->frames.front().instances.end();iter++) {
        if (iter->type==SYMBOL_INSTANCE) {
            iter->bVisible = iter->name == selectedName;
        }
    }
    
    
    ofVec2f p1 = videoMat.preMult(ofVec3f());
    ofVec2f p2 = videoMat.preMult(ofVec3f(WINDOW_WIDTH,WINDOW_HEIGHT));
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
        
        float scale = max(images[imageNum].getWidth()/window.width,images[imageNum].getHeight()/window.height);
        cam.setup(window,window.width*scale,window.height*scale,mat);
        cam.setMinZoom(iter->minZoom);
        cam.setMaxZoom(iter->maxZoom);
        
    } else {
    
        cam.setup(window, images[imageNum].getWidth(), images[imageNum].getHeight());
        cam.setMinZoom(WINDOW_WIDTH/(float)images[imageNum].getWidth());
        cam.setMaxZoom(2.0f);
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
        if (images[imageNum].getDidLoad()) {
            images[imageNum].unload();
        }
        updateOverlays();
    }
    
    if (state==STATE_IDLE && !player.getIsPlaying()) {
        player.play(ofToDataPath("video.mov"));
    }
    
#endif
}


void kaiserNav::draw2nd() {
    ofPushMatrix();
    
    switch (state) {
        case STATE_NAVIGATION:
        case STATE_TUTORIAL:
            ofPushMatrix();
            glMultMatrixf(extMat.getPtr());
            ofTranslate(ofVec2f(-0.5*ofVec2f(images[imageNum].getWidth(),images[imageNum].getHeight())));
            images[imageNum].draw();
//            cam.end();	//back to normal ofSetupScreen() projection
            ofPopMatrix();
            
            ofEnableAlphaBlending();
            screenLayout.draw();
            
            
            if (bCaptionActive) {
                extMarker.draw();
                ofPushStyle();
                ofSetColor(184,41,35,floating.getFade()*255);
                ofSetLineWidth(2);
                ofLine(linep1,linep2);
                
                ofPopStyle();
                extCaption.draw();
                
            }
            
            break;
        
            
        case STATE_IDLE:
#ifdef TARGET_OF_IPHONE
            player.draw(ofRectangle(0,0,player.getWidth(),player.getHeight()));
#endif           
            break;
        default:
            break;
    }
    
    
    

    
    
    
    
    ofPopMatrix();
    
}

void kaiserNav::draw() {
    //    deep.begin();
    //    deep.draw();
    //    deep.end();
    
    ofPushMatrix();
    
	cam.begin(); //put all our drawing under the ofxPanZoom effect
       
    ofTranslate(-0.5*ofVec2f(images[imageNum].getWidth(),images[imageNum].getHeight()));

    ofDisableAlphaBlending();
    images[imageNum].draw();
    ofEnableAlphaBlending();
    
    imageLayout.draw();
    
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
    
#ifdef TARGET_OF_IPHONE
    if (state==STATE_IDLE) {
//        float offset = 0.5*(ofGetWidth()-player.getWidth());
//        player.draw(ofRectangle(offset,offset,player.getWidth(),player.getHeight()));
        ofPushMatrix();
        glMultMatrixf(videoMat.getPtr());
        ofScale(EXTERNAL_ZOOM_RECIP, EXTERNAL_ZOOM_RECIP);
        player.draw(ofRectangle(0,0,player.getWidth(),player.getHeight()));
        ofPopMatrix();
    }
#endif
    
    interfaceLayout.draw();
    
    if (bCaptionActive) {
        floating.draw();
        caption.draw();
    }
    
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
            player.stop();
            setImage(imageName);
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
            
            vector<ofxSymbolInstance *> hits;
            
            if (bCaptionActive && caption.hitTest(ofVec2f(touch.x,touch.y),hits)) {
                bCaptionActive =false;
            }
            
            hits.clear();
            for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
                
                if (iter->second.hitTest(ofVec2f(touch.x,touch.y),hits)) {
                    cout << iter->first->name << endl;
                    setCaption(iter->first->name);
                }
            }
            
//            ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
//            titles->bVisible = false;
            hits.clear();
            if (interfaceLayout.hitLayer(interfaceLayout.getLayer("thumbs"),ofVec2f(touch.x,touch.y),hits)) {
                if (hits.front()->name != imageName) {
                    setImage(hits.front()->name);
                }
            }
            
            
            hits.clear();
            if(interfaceLayout.hitLayer(interfaceLayout.getLayer("language"),ofVec2f(touch.x,touch.y),hits)) {
                if (hits.front()->name.size()==2) {
                    lang = hits.front()->name;
                    if (bCaptionActive) {
                        setCaption(captionName);
                    }
                }
            }
            
            hits.clear();
            if( interfaceLayout.getChild("LANGUAGE")->hitTest(interfaceLayout.mat.getInverse().preMult(ofVec3f(touch.x,touch.y)),hits)) {
                if (hits.front()->name!=lang+"_S") {
                    lang = hits.front()->name;
                    if (bCaptionActive) {
                        setCaption(captionName);
                    }
                }
            }
            
            
            
            hits.clear();
            if( interfaceLayout.getChild("titles")->hitTest(interfaceLayout.mat.getInverse().preMult(ofVec3f(touch.x,touch.y)),hits)) {
                for (vector<ofxSymbolInstance *>::iterator iter = hits.begin();iter!=hits.end();iter++) {
                    if ((*iter)->name == "MARKER_MINUSE_"+lang || (*iter)->name == "MARKER_PLUS_"+lang) {
                        bSubTitle = !bSubTitle;
                        bCaptionActive = false;
                        break;
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
    
//    else {
//        if (imageName=="I2") {
//            setImage("I1");
//        } else {
//            setImage("I2");
//        }
//    }
    
}


