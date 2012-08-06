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
    mat.scale(0.5, 0.5, 1.0);
    layout = doc.getSymbolItem("Layout")->createInstance("layout",mat);
    setTool(BRUSH_TOOL);
    layout.update();
    
    
    //    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
    //        layout.mat.translate(64, 0, 0);
    //        layout.mat.scale(5.0/6.0, 5.0/6.0, 1.0);
    //    }
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    
   
    
}



//--------------------------------------------------------------
void silentNature::draw(){
	ofSetColor(255);
    
    layout.draw();
    
    for (vector<stroke>::iterator iter=strokes.begin();iter!=strokes.end();iter++) {
        ofSetHexColor(iter->color);
        ofPushMatrix();
        glMultMatrixf(iter->mat.getPtr());
        switch (iter->tool) {
            case BRUSH_TOOL:
            case ERASER_TOOL:
                ofCircle(ofVec2f(0,0), 30);
                break;
            case CRAYON_TOOL:
            case CUTOUT_TOOL: {
                ofRectangle rect;
                rect.setFromCenter(ofVec2f(0,0),60,60);
                ofRect(rect);
            }
            break;
            default:
                break;
        }
        ofPopMatrix();
    }
    
}

//--------------------------------------------------------------
void silentNature::exit(){
    
    
}

//--------------------------------------------------------------
void silentNature::touchDown(ofTouchEventArgs &touch){
    
    ofVec2f pos = ofVec2f(touch.x,touch.y);
    
    vector<ofxSymbolInstance> items = layout.hitTest(pos);
    
    for (vector<ofxSymbolInstance>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if (iter->type == SYMBOL_INSTANCE && iter->bVisible == true) {
//            cout << iter->name << "\t";
            if (iter->name == "palette") {
                setTool(BRUSH_TOOL);
                break;
            }
            
            if (iter->name == "crayons") {
                setTool(CRAYON_TOOL);
                break;
            }
            
            if (iter->name == "paper") {
                setTool(CUTOUT_TOOL);
                break;
            }
            
            if (iter->name == "eraser") {
                setTool(ERASER_TOOL);
                break;
            }
            
            if (iter->name == "canvas") {
                applyTool(pos);
            }
            
            if (iter->name == "clear") {
                strokes.clear();
            }

            
        }
    }
    
    layout.update();
    
    cout << endl;
}

//--------------------------------------------------------------
void silentNature::touchMoved(ofTouchEventArgs &touch){
    ofVec2f pos = ofVec2f(touch.x,touch.y);
    
    vector<ofxSymbolInstance> items = layout.hitTest(pos);
    
    for (vector<ofxSymbolInstance>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if (iter->type == SYMBOL_INSTANCE && iter->bVisible == true) {
            
                        
            if (iter->name == "canvas") {
                applyTool(pos);
            }
            
            
        }
    }

}

//--------------------------------------------------------------
void silentNature::touchUp(ofTouchEventArgs &touch){
    
}

//--------------------------------------------------------------
void silentNature::touchDoubleTap(ofTouchEventArgs &touch){
    
}



void silentNature::setTool(int tool) {
    layout.getChild("palette")->alphaMultiplier = 0.5;
    layout.getChild("crayons")->alphaMultiplier = 0.5;
    layout.getChild("paper")->alphaMultiplier = 0.5;
    layout.getChild("eraser")->alphaMultiplier = 0.5;

    this->tool = tool;
    
    switch (tool) {
        case BRUSH_TOOL:
            layout.getChild("palette")->alphaMultiplier = 1.0;
            break;
            
        case CRAYON_TOOL:
            layout.getChild("crayons")->alphaMultiplier = 1.0;
            
            break;
            
        case CUTOUT_TOOL:
            layout.getChild("paper")->alphaMultiplier = 1.0;
            
            break;
            
        case ERASER_TOOL:
            layout.getChild("eraser")->alphaMultiplier = 1.0;
            break;
            
        default:
            break;
    }
    
}

void silentNature::applyTool(ofVec2f pos) {
    ofMatrix4x4 mat;
    
    int color = 0x000000;
    
    
    switch (tool) {
        case BRUSH_TOOL:
            mat.scale(0.3, 0.3, 1.0);
            break;
            
        case ERASER_TOOL:
            mat.scale(0.3, 0.3, 1.0);
            color = 0xffffff;
            break;
        case CRAYON_TOOL:
            mat.scale(0.3, 0.3, 1.0);
            break;
            
        case CUTOUT_TOOL: {
            color = ofRandomf()>0.5 ? 0xff0000 : 0x0000ff;
            float scale = ofRandom(0.75, 1.00);
            mat.scale(scale, scale, 1.0);
            mat.rotate(ofRandom(90), 0.0, 0.0, 1.0);
        }    break;
            
        default:
            break;
    }             
    
    mat.translate(pos);
    
    strokes.push_back(stroke(tool, mat, color));
}
