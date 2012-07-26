//
//  kaiserNav.cpp
//  kaiserNav
//
//  Created by Roee Kremer on 7/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "kaiserNav.h"


int limitX = 6000;	//these define where the camera can pan to
int limitY = 4000;

ofVec2f kaiserNav::camOffset( ){
	
    //return (p-0.5*ofVec2f(limitX,limitY)+cam.offset)*cam.zoom+0.5f*ofVec2f(ofGetWidth(),ofGetHeight());
    return cam.offset*cam.zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
}

void kaiserNav::updateMarkers() {
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance> >::iterator iter=markers.begin();iter!=markers.end();iter++) {
        iter->second.mat.makeTranslationMatrix(cam.worldToScreen(iter->first->mat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height)));
    }
    
    deep.transform( camOffset(), cam.zoom);
    floating.setAnchor(cam.worldToScreen(captionMat.preMult(ofVec3f(0,0,0))-0.5*ofVec2f(width,height))); 
    caption.mat.makeTranslationMatrix(floating.getPos());
}

void kaiserNav::setup(){	
	
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
    ofMatrix4x4 mat;
    
    layout = doc.getSymbolItem("LAYOUT")->createInstance("layout", mat);
    
   
    for (vector<layer>::iterator liter=layout.layers.begin(); liter!=layout.layers.end(); liter++) {
        if (liter->name!="interface") {
            for (vector<ofxSymbolInstance>::iterator iter=liter->instances.begin(); iter!=liter->instances.end();iter++) {
                if (iter->type==SYMBOL_INSTANCE) {
                    iter->bVisible = false;
                }
            }
        }
    }
    
    
    image = doc.getSymbolItem("IMAGE_1")->createInstance("image", mat);
    
    layer *markersLayer = image.getLayer("markers");
    
    for (vector<ofxSymbolInstance>::iterator iter=markersLayer->instances.begin(); iter!=markersLayer->instances.end();iter++) {
        if (iter->type==SYMBOL_INSTANCE) {
            ofMatrix4x4 mat;
            ofxSymbolInstance marker = doc.getSymbolItem("MARKER_IMAGE")->createInstance(iter->name,mat);
            markers.push_back(make_pair(&*iter,marker));
            cout << iter->name << endl;
        }
    }
     
    //    layout.getChild("pimp")->bVisible = false;
    
    captionMat = mat;
    captionMat.preMult(image.getChild("I1_B1")->mat);
    caption = doc.getSymbolItem("I1_B3_EN")->createInstance("caption", captionMat);
    
    
    //    layout.getChild("pimp")->bVisible = false;
    
	
	ofEnableAlphaBlending();
	
	ofSetCircleResolution(32);
	
	cam.setZoom(0.125f);
    //    cam.setZoom(1.0f);
	cam.setMinZoom(0.1f);
	cam.setMaxZoom(2.0f);
	cam.setScreenSize( ofGetWidth(), ofGetHeight() );
	cam.setViewportConstrain( ofVec3f(-limitX, -limitY), ofVec3f(limitX, limitY)); //limit browseable area, in world units
	
    //    deep.setup("IMAGE_1", "png",5053, 3517, ofRectangle(100,100, ofGetWidth()-200,ofGetHeight()-200));
    width = 5053;
    height = 3517;
    deep.setup("IMAGE_1", "png",width, height, ofRectangle(0,0, ofGetWidth(),ofGetHeight()));
    deep.transform( camOffset(), cam.zoom);
    
    deep.start();
    
	floating.setup(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()), 150, 150);
    
    updateMarkers();
    
    
}


void kaiserNav::update() {
    cam.update();
    
    if (cam.getIsAnimating()) {
        
        
        updateMarkers();
        
    }
    
	deep.update();
}


void kaiserNav::draw() {
    //    deep.begin();
    //    deep.draw();
    //    deep.end();
    
    
    
	cam.apply(); //put all our drawing under the ofxPanZoom effect
    ofPushMatrix();
    
    
    deep.draw();
    
    
    
    image.draw();
    
    
    ofPopMatrix();
    
    
    
    
    //draw grid
    
	
    //draw space constrains		
    int s = 25;
    glColor4f(1, 0, 0, 1);
    ofRect(-limitX , -limitY , 2 * limitX, s);
    ofRect(limitX - s , -limitY , s, 2 * limitY);
    ofRect(-limitX , limitY - s , s, -2 * limitY);	
    ofRect(limitX , limitY - s, -2 * limitX, s);		
    glColor4f(1, 1, 1, 1);
	
	cam.reset();	//back to normal ofSetupScreen() projection
	
    ofPushStyle();
    ofSetColor(0);
    ofSetLineWidth(2);
    ofVec2f vec = floating.getAnchor()-floating.getPos();
//    vec = vec.normalized()*(vec.length()-200*cam.zoom);
    vec = vec.normalized()*(vec.length()-13.5);
    ofLine(floating.getPos(), floating.getPos()+vec);
    ofPopStyle();
    caption.draw();
    for (vector<pair<ofxSymbolInstance *,ofxSymbolInstance> >::iterator iter=markers.begin();iter!=markers.end();iter++) {
        iter->second.draw();
    }
    
    layout.draw();
    
	cam.drawDebug(); //see info on ofxPanZoom status
}

void kaiserNav::exit() {
    //    deep.stop();
}

void kaiserNav::touchDown(ofTouchEventArgs &touch){
    
	cam.touchDown(touch); //fw event to cam
	
	ofVec3f p =  cam.screenToWorld( ofVec3f( touch.x, touch.y) );	//convert touch (in screen units) to world units
	
}


void kaiserNav::touchMoved(ofTouchEventArgs &touch){

	cam.touchMoved(touch); //fw event to cam
    updateMarkers();
}


void kaiserNav::touchUp(ofTouchEventArgs &touch){
	cam.touchUp(touch);	//fw event to cam
}


void kaiserNav::touchDoubleTap(ofTouchEventArgs &touch){
	cam.touchDoubleTap(touch); //fw event to cam
	cam.setZoom(1.0f);	//reset zoom
	cam.lookAt( ofVec3f() ); //reset position
}


