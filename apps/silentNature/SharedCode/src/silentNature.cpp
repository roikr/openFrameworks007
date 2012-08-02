//
//  silentNature.cpp
//  iSilentNature
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "silentNature.h"



//--------------------------------------------------------------
void silentNature::setup(){	
	
    
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
	ofBackground(127,127,127);
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
    ofMatrix4x4 mat;
    layout = doc.getSymbolItem("Layout")->createInstance("layout",mat);
    
    
    //    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
    //        layout.mat.translate(64, 0, 0);
    //        layout.mat.scale(5.0/6.0, 5.0/6.0, 1.0);
    //    }
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    
    setTool(BRUSH_TOOL);
    
}



//--------------------------------------------------------------
void silentNature::draw(){
	ofSetColor(255);
    
    layout.draw();
    
    
}

//--------------------------------------------------------------
void silentNature::exit(){
    
    
}

//--------------------------------------------------------------
void silentNature::touchDown(ofTouchEventArgs &touch){
    vector<ofxSymbolInstance> items = layout.hitTest(ofVec2f(touch.x,touch.y));
    
    for (vector<ofxSymbolInstance>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if (iter->type == SYMBOL_INSTANCE && iter->bVisible == true) {
            cout << iter->name << "\t";
            if (iter->name == "brush_closed") {
                setTool(BRUSH_TOOL);
                break;
            }
            
            if (iter->name == "crayons_closed") {
                setTool(CRAYON_TOOL);
                break;
            }
            
            if (iter->name == "cutouts_closed") {
                setTool(CUTOUT_TOOL);
                break;
            }
            
        }
    }
    
    cout << endl;
}

//--------------------------------------------------------------
void silentNature::touchMoved(ofTouchEventArgs &touch){
    
}

//--------------------------------------------------------------
void silentNature::touchUp(ofTouchEventArgs &touch){
    
}

//--------------------------------------------------------------
void silentNature::touchDoubleTap(ofTouchEventArgs &touch){
    
}



void silentNature::setTool(int tool) {
    layout.getChild("brush_closed")->bVisible = true;
    layout.getChild("crayons_closed")->bVisible = true;
    layout.getChild("cutouts_closed")->bVisible = true;
    layout.getChild("brush_open")->bVisible = false;
    layout.getChild("colors")->bVisible = false;
    layout.getChild("crayons_open")->bVisible = false;
    layout.getChild("patterns")->bVisible = false;
    
    this->tool = tool;
    
    switch (tool) {
        case BRUSH_TOOL:
            layout.getChild("brush_open")->bVisible = true;
            layout.getChild("colors")->bVisible = true;
            layout.getChild("brush_closed")->bVisible = false;
            break;
            
        case CRAYON_TOOL:
            layout.getChild("crayons_open")->bVisible = true;
            layout.getChild("crayons_closed")->bVisible = false;
            break;
            
        case CUTOUT_TOOL:
            layout.getChild("patterns")->bVisible = true;
            layout.getChild("cutouts_closed")->bVisible = false;
            break;
            
        default:
            break;
    }
    
}