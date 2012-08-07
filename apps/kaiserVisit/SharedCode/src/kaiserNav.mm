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
    for (vector<pair<ofxSymbolInstance *,pair<ofxSymbolInstance,ofxSymbolInstance> > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
//        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height)));
        iter->second.first.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))+cam.topLeftConstrain));
        iter->second.second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))+cam.topLeftConstrain));
    }
    
    ofVec2f camOffset = cam.offset*cam.zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
        
    if (bCaptionActive) {
        
        
        
        ofxSymbolInstance *child = imageLayout.getChild(caption.name);
        ofMatrix4x4 mat; // should initialized to general transform if any exist
        mat.preMult(child->mat); // we set the caption name to the correspond marker when we create it
                
        floating.setAnchor(cam.worldToScreen(mat.preMult(ofVec3f(0,0,0))+cam.topLeftConstrain)); 
        
        ofRectangle rect = caption.getBoundingBox();
        
//        cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;

        caption.mat.makeTranslationMatrix(floating.getPos()-0.5*ofVec2f(rect.width,rect.height));
        caption.alphaMultiplier = floating.getFade();
        caption.update();
        if (floating.getFade()<=0) {
            bCaptionActive = false;
        }
        
        
    }
}

void kaiserNav::setCaption(string name) {
    bCaptionActive = true;
    
    captionName = name;
    
    caption = doc.getSymbolItem(captionName+'_'+lang)->createInstance(name);
    ofRectangle rect = caption.getBoundingBox();
    floating.setup(ofRectangle(35, 35, ofGetWidth()-70, 535), rect, 150);
    updateOverlays();
}

void kaiserNav::setup(){	
	
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
   
    
    interfaceLayout = doc.getSymbolItem("LAYOUT")->createInstance("layout");
    
   
	lang = "HE";
    setImage("I1");	
    
    
	
	ofSetCircleResolution(32);

    state = STATE_IDLE;
    setState(STATE_IDLE);
    
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
            ofMatrix4x4 mat;
            ofxSymbolInstance marker = doc.getSymbolItem("MARKER_IMAGE")->createInstance(iter->name,mat);
            ofxSymbolInstance screenMarker = doc.getSymbolItem("MARKER_SCREEN")->createInstance(iter->name,mat);
            markers.push_back(make_pair(&*iter,make_pair(marker,screenMarker)));
            cout << iter->name << endl;
        }
    }
    
    
    ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
    titles->bVisible = true;
    string titleName = imageName+"_C_"+lang;
    
    for (vector<layer>::iterator liter=titles->layers.begin(); liter!=titles->layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->instances.begin(); iter!=liter->instances.end();iter++) {
            if (iter->type==SYMBOL_INSTANCE) {
                iter->bVisible = iter->name == titleName;
            }
        }
    }
    
    float minZoom = 1024.0/(float)image.getWidth();
    //	cam.setZoom(0.125f);
    cam.setZoom(minZoom*2);
	cam.setMinZoom(minZoom);
	cam.setMaxZoom(2.0f);
	cam.setScreenSize( ofGetWidth(), ofGetHeight() );
    ofVec2f limit = 0.5*ofVec2f(image.getWidth(),image.getHeight());
    cam.setViewportConstrain( -limit, limit); //limit browseable area, in world units
	
    
    bCaptionActive = false;
    
    updateOverlays();
}

void kaiserNav::setState(int state) {
    
    interfaceLayout.getChild("idle")->bVisible = false;
    interfaceLayout.getChild("tutorial")->bVisible = false;
    
    switch (this->state) {
        case STATE_IDLE:
            
            break;
            
        default:
            break;
    }
    
    this->state = state;
    
    switch (this->state) {
        case STATE_IDLE:
            player.play(ofToDataPath("Idle_test.mov"));
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
    player.update();
    if (cam.getIsAnimating()) {
        updateOverlays();
    }

}


void kaiserNav::draw2nd() {
    ofPushMatrix();
 	cam.apply(); //put all our drawing under the ofxPanZoom effect
    ofPushMatrix();
    
    ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
    image.draw();
    
    imageLayout.draw();
    
    ofPopMatrix();

    int s = 25;
    glColor4f(1, 0, 0, 1);
    float limitX = cam.bottomRightConstrain.x;
    float limitY = cam.bottomRightConstrain.y;
    ofRect(-limitX , -limitY , 2 * limitX, s);
    ofRect(limitX - s , -limitY , s, 2 * limitY);
    ofRect(-limitX , limitY - s , s, -2 * limitY);	
    ofRect(limitX , limitY - s, -2 * limitX, s);		
    glColor4f(1, 1, 1, 1);
	
	cam.reset();	//back to normal ofSetupScreen() projection
    
    if (state==STATE_NAVIGATION) {
        for (vector<pair<ofxSymbolInstance *,pair<ofxSymbolInstance,ofxSymbolInstance> > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
            iter->second.second.draw();
        }
    }
	
    if (bCaptionActive) {
        floating.draw();
        caption.draw();
    }
    ofPopMatrix();
    
    if (state==STATE_IDLE) {
        float offset = 0.5*(ofGetWidth()-player.getWidth());
        player.draw(ofRectangle(offset,offset,player.getWidth(),player.getHeight()));
    }
    
}

void kaiserNav::draw() {
    //    deep.begin();
    //    deep.draw();
    //    deep.end();
    
    ofPushMatrix();
    
	cam.apply(); //put all our drawing under the ofxPanZoom effect
    ofPushMatrix();
    
    
    ofTranslate(-0.5*ofVec2f(image.getWidth(),image.getHeight()));
    ofDisableAlphaBlending();
    image.draw();
    ofEnableAlphaBlending();
    
    imageLayout.draw();
    
    
    ofPopMatrix();
    
    //draw space constrains	
	float limitX = cam.bottomRightConstrain.x;
    float limitY = cam.bottomRightConstrain.y;
    int s = 25;
    glColor4f(1, 0, 0, 1);
    ofRect(-limitX , -limitY , 2 * limitX, s);
    ofRect(limitX - s , -limitY , s, 2 * limitY);
    ofRect(-limitX , limitY - s , s, -2 * limitY);	
    ofRect(limitX , limitY - s, -2 * limitX, s);		
    glColor4f(1, 1, 1, 1);
	
	cam.reset();	//back to normal ofSetupScreen() projection
	
    if (state==STATE_NAVIGATION) {
        for (vector<pair<ofxSymbolInstance *,pair<ofxSymbolInstance,ofxSymbolInstance> > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
            iter->second.first.draw();
        }
    }
    
    
    
    if (bCaptionActive) {
        floating.draw();
        caption.draw();
    }
    
    if (state==STATE_IDLE) {
        float offset = 0.5*(ofGetWidth()-player.getWidth());
        player.draw(ofRectangle(offset,offset,player.getWidth(),player.getHeight()));
    }
    
    interfaceLayout.draw();
    
    ofPopMatrix();
//	cam.drawDebug(); //see info on ofxPanZoom status
}

void kaiserNav::exit() {
}

void kaiserNav::touchDown(ofTouchEventArgs &touch){
    
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
            for (vector<pair<ofxSymbolInstance *,pair<ofxSymbolInstance,ofxSymbolInstance> > >::iterator iter=markers.begin();iter!=markers.end();iter++) {
                
                if (!iter->second.first.hitTest(ofVec2f(touch.x,touch.y)).empty()) {
                    cout << iter->first->name << endl;
                    setCaption(iter->first->name);
                }
            }
            
            ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
            titles->bVisible = false;
            
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
            
            if (!caption.hitTest(ofVec2f(touch.x,touch.y)).empty()) {
                bCaptionActive =false;
            }

        } break;
        default:
            break;
    }
    

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
}


