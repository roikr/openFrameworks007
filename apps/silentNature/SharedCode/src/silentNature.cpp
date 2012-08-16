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
    float scale = (float)ofGetHeight()/1920.0;
    mat.scale(scale, scale, 1.0);
    mat.translate(0.5*(ofGetWidth()-scale*1080.0), 0, 0);
    layout = doc.getSymbolItem("Layout")->createInstance("layout",mat);
    
    setTool(BRUSH_TOOL);
    
    
    canvas = layout.getChild("canvas");
    
    layout.getChildMat(canvas, cmat);
  
    //cmat.translate(canvas->mat.getTranslation());
        
    
    ofRectangle rect = canvas->getBoundingBox();
    rect.width = 1024;
    rect.height = 1024;
    cout << rect.width << "\t" << rect.height << endl;
    canvasTex.allocate(rect.width, rect.height, GL_RGBA);
    //canvasTex.texData.bFlipTexture = true;
    fbo.setup(rect.width, rect.height);
    
    fbo.begin(canvasTex.getTextureData().textureID);
    glClearColor(0,0,0, 0);
    glClear( GL_COLOR_BUFFER_BIT);
    fbo.end();
    
    
    
    //    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
    //        layout.mat.translate(64, 0, 0);
    //        layout.mat.scale(5.0/6.0, 5.0/6.0, 1.0);
    //    }
    
//    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    
   
    bDown = false;
}




//--------------------------------------------------------------
void silentNature::update(){
    
    
}

void silentNature::draw(){    
	ofSetColor(255);
    
    
    ofPushMatrix();
    layout.draw();
    ofPopMatrix();
    
    
    ofPushMatrix();
    glMultMatrixf(cmat.getPtr())    ;
    ofSetColor(255);
    canvasTex.draw(0, 0);
    if (bDown) {
        switch (tool) {
            case BRUSH_TOOL:
                stroke.draw();
                break;
                
        }
    }
    ofPopMatrix();
    
    
    
   
    
}

//--------------------------------------------------------------
void silentNature::exit(){
    
    
}

//--------------------------------------------------------------
void silentNature::touchDown(ofTouchEventArgs &touch){
    
    ofVec2f pos = ofVec2f(touch.x,touch.y);
    
    vector<ofxSymbolInstance*> items = layout.hitTest(pos);
    
    for (vector<ofxSymbolInstance*>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if ((*iter)->type == SYMBOL_INSTANCE && (*iter)->bVisible == true) {
//            cout << iter->name << "\t";
            if ((*iter)->name == "palette") {
                setTool(BRUSH_TOOL);
                break;
            }
            
            if ((*iter)->name == "crayons") {
                setTool(CRAYON_TOOL);
                break;
            }
            
            if ((*iter)->name == "paper") {
                setTool(CUTOUT_TOOL);
                break;
            }
            
            if ((*iter)->name == "eraser") {
                setTool(ERASER_TOOL);
                break;
            }
            
            if ((*iter)->name == "canvas") {
                bDown = true;
                ofVec2f cpos = cmat.getInverse().preMult(ofVec3f(pos));
                
                switch (tool) {
                    case BRUSH_TOOL:
                        
                        stroke.setup(30, ofColor(0));
                        touches.push_back(cpos);
                        break;
                        
                    default:
                        break;
                }
                
            }
            
            if ((*iter)->name == "clear") {
                fbo.begin(canvasTex.getTextureData().textureID);
                glClearColor(0,0,0, 0);
                glClear( GL_COLOR_BUFFER_BIT);
                fbo.end();
                            }

            
        }
    }
    
    
}

//--------------------------------------------------------------
void silentNature::touchMoved(ofTouchEventArgs &touch){
    ofVec2f pos = ofVec2f(touch.x,touch.y);
    
    vector<ofxSymbolInstance*> items = layout.hitTest(pos);
    
    for (vector<ofxSymbolInstance*>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if ((*iter)->type == SYMBOL_INSTANCE && (*iter)->bVisible == true) {
            
                        
            if ((*iter)->name == "canvas") {
                ofVec2f cpos = cmat.getInverse().preMult(ofVec3f(pos));
                
                switch (tool) {
                    case BRUSH_TOOL:
                        touches.push_back(cpos);
                        if (touches.size()==4) {
                            stroke.addPatch(vector<ofVec2f>(touches.begin(),touches.end()));
                            touches.pop_front();
                        }
                        
                        break;
                    case ERASER_TOOL:
                        fbo.begin(canvasTex.getTextureData().textureID);
                        
                        ofDisableAlphaBlending();
                        ofSetColor(0 , 0, 0, 0);
                        
                        ofEllipse(cpos, ofRandom(50, 70), ofRandom(50, 70));
                        ofEnableAlphaBlending();
                        fbo.end();
                        
                        break;
                        
                    default:
                        break;
                }
            }
            
            
        }
    }

}

//--------------------------------------------------------------
void silentNature::touchUp(ofTouchEventArgs &touch){
    if (bDown) {
        switch (tool) {
            case BRUSH_TOOL:
                touches.clear();
                fbo.begin(canvasTex.getTextureData().textureID);
                stroke.draw();
                fbo.end();
                
                break;
                
            default:
                break;
        }
        bDown = false;
    }
    
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

/*
void silentNature::applyTool(ofVec2f pos) {
    ofMatrix4x4 mat;
    
    int color = 0x000000;
    
    
    switch (tool) {
        case BRUSH_TOOL:
            mat.scale(0.3, 0.3, 1.0);
            break;
            
        case ERASER_TOOL:
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
    
    
    
    
    fbo.begin(canvasTex.getTextureData().textureID);
    
    ofSetHexColor(color);
    
    
    ofPushMatrix();
    glMultMatrixf(mat.getPtr());
    switch (tool) {
        case BRUSH_TOOL:
           
            ofCircle(ofVec2f(0,0), 30);
            break;
        case ERASER_TOOL: 
            //drawTexture(1.0,1.0);
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
    
    fbo.end();
    
}
*/