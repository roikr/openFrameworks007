//
//  kaiserNav.cpp
//  kaiserNav
//
//  Created by Roee Kremer on 7/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "kaiserNav.h"
#include "ofxXmlSettings.h"



void kaiserNav::updateOverlays() {
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance> >::iterator iter=markers.begin();iter!=markers.end();iter++) {
//        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height)));
        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))+cam.topLeftConstrain));
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
        ofVec2f vec = floating.getAnchor()-floating.getPos();
        vec.normalize();
        float a = vec.angle(ofVec2f(1,0))*PI/180;
        captionLength = MIN(rect.width/(2.0*abs(cos(a))),rect.height/(2.0*abs(sin(a))));
        cout << a*180/PI << "\t" << captionLength << endl;
    }
}

void kaiserNav::setCaption(string name) {
    bCaptionActive = true;
    
    captionName = name;
    
    caption = doc.getSymbolItem(captionName+'_'+lang)->createInstance(name);
    ofRectangle rect = caption.getBoundingBox();
    floating.setup(ofRectangle(35, 35, ofGetWidth()-70, 535), 0.5*rect.width,0.5*rect.height, 150);
    updateOverlays();
}

void kaiserNav::setup(){	
	
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
   
    
    interfaceLayout = doc.getSymbolItem("LAYOUT")->createInstance("layout");
    
   
	lang = "HE";
    setImage("I1");	
    
    
	
	ofSetCircleResolution(32);

    
}

void kaiserNav::setLanguage(string lang) {
    this->lang = lang;
    
    ofxSymbolInstance *titles = interfaceLayout.getChild("titles");
    string titleName = imageName+"_C_"+lang;
    
    for (vector<layer>::iterator liter=titles->layers.begin(); liter!=titles->layers.end(); liter++) {
        for (vector<ofxSymbolInstance>::iterator iter=liter->instances.begin(); iter!=liter->instances.end();iter++) {
            if (iter->type==SYMBOL_INSTANCE) {
                iter->bVisible = iter->name == titleName;
            }
        }
    }
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
            markers.push_back(make_pair(&*iter,marker));
            cout << iter->name << endl;
        }
    }
    
    setLanguage(lang);
    
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


void kaiserNav::update() {
    cam.update();
    
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
	
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance> >::iterator iter=markers.begin();iter!=markers.end();iter++) {
        iter->second.draw();
    }
    
    if (bCaptionActive) {
        ofPushStyle();
        ofSetColor(0);
        ofSetLineWidth(2);
        ofVec2f vec = floating.getAnchor()-floating.getPos();
        float totalLen = vec.length();
        vec.normalize();
        
        ofLine(floating.getPos()+vec*(captionLength+20), floating.getPos()+vec*(totalLen-13.5-20));
        ofPopStyle();
        caption.draw();
    }
    ofPopMatrix();
    
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
	
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance> >::iterator iter=markers.begin();iter!=markers.end();iter++) {
        iter->second.draw();
    }
    
    
    
    if (bCaptionActive) {
        ofPushStyle();
        ofSetColor(0);
        ofSetLineWidth(2);
        ofVec2f vec = floating.getAnchor()-floating.getPos();
        float totalLen = vec.length();
        vec.normalize();
        
        ofLine(floating.getPos()+vec*(captionLength+20), floating.getPos()+vec*(totalLen-13.5-20));
        ofPopStyle();
        caption.draw();
    }
    
    
    interfaceLayout.draw();
    
    ofPopMatrix();
	cam.drawDebug(); //see info on ofxPanZoom status
}

void kaiserNav::exit() {
}

void kaiserNav::touchDown(ofTouchEventArgs &touch){
    
	cam.touchDown(touch); //fw event to cam
	
//	ofVec2f p =  cam.screenToWorld( ofVec3f( touch.x, touch.y) ) + 0.5*ofVec2f(width,height);	//convert touch (in screen units) to world units
	
    ofVec2f p(touch.x,touch.y);
    
//    cout << p.x << "\t" << p.y << endl;
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance> >::iterator iter=markers.begin();iter!=markers.end();iter++) {
        
        if (!iter->second.hitTest(ofVec2f(touch.x,touch.y)).empty()) {
            cout << iter->first->name << endl;
            setCaption(iter->first->name);
        }
    }
    
    
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
            setLanguage(iter->name);
            if (bCaptionActive) {
                setCaption(captionName);
            }
        }
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
	cam.touchDoubleTap(touch); //fw event to cam
	cam.setZoom(1.0f);	//reset zoom
	cam.lookAt( ofVec3f() ); //reset position
}


