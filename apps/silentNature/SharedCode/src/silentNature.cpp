//
//  silentNature.cpp
//  iSilentNature
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "silentNature.h"


#define BRUSH_THICKNESS 30.0
#define CRAYON_THICKNESS 15.0
#define ERASER_THICKNESS 60.0

#ifdef TARGET_OPENGLES
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#endif

//--------------------------------------------------------------
void silentNature::setup(){	
	
    
	
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetLineWidth(5);
    
	ofBackground(127,127,127);
    
    doc.setup("DOMDocument.xml");
    doc.load();
    
    ofMatrix4x4 mat;
    float scale = (float)ofGetHeight()/1920.0;
    mat.scale(scale, scale, 1.0);
    mat.translate(0.5*(ofGetWidth()-scale*1080.0), 0, 0);
    layout = doc.getSymbolItem("Layout")->createInstance("layout",mat);
    
    ofImage &image = doc.getBitmapItem("images/magenta")->getImage();
   
    tool = BRUSH_TOOL;
    color = image.getPixelsRef().getColor(image.getWidth()/2, image.getHeight()/2);
    paper = NULL;
            
    canvas = layout.getChild("canvas");
    
    layout.getChildMat(canvas, cmat);
  
    //cmat.translate(canvas->mat.getTranslation());
    
#ifdef TARGET_OPENGLES
    canvasTex.allocate(1024, 1024, GL_RGBA);
    //canvasTex.texData.bFlipTexture = true;
    fbo.setup(1024, 1024);
#else
    ofRectangle rect = canvas->getBoundingBox();
    cout << rect.width << "\t" << rect.height << endl;
    fbo.allocate(rect.width, rect.height);
#endif

#ifdef TARGET_OPENGLES
    fbo.begin(canvasTex.getTextureData().textureID);
#else
    fbo.begin();
#endif
    glClearColor(0,0,0, 0);
    glClear( GL_COLOR_BUFFER_BIT);
    fbo.end();
    
    //    if (iPhoneGetDeviceType() == OFXIPHONE_DEVICE_IPHONE) {
    //        layout.mat.translate(64, 0, 0);
    //        layout.mat.scale(5.0/6.0, 5.0/6.0, 1.0);
    //    }
    
//    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    
    bDown = false;
    
    //action_url = "http://localhost:8888/postImage.of";
	ofAddListener(httpUtils.newResponseEvent,this,&silentNature::newResponse);
	httpUtils.start();
    counter = 0;

}




//--------------------------------------------------------------
void silentNature::update(){
    
    
}

void silentNature::drawTool() {
    
   
    
    switch (tool) {
        case BRUSH_TOOL: {
            ofSetColor(color);
            vector<ofVec2f> &curve = stroke.getCurve();
            for (vector<ofVec2f>::iterator iter=curve.begin();iter!=curve.end();iter++) {
                ofCircle(*iter, BRUSH_THICKNESS/2);
            }
                    
        } break;
        case CRAYON_TOOL: {
            ofSetColor(color);
            vector<ofVec2f> &curve = stroke.getCurve();
            for (vector<ofVec2f>::iterator iter=curve.begin();iter!=curve.end();iter++) {
                ofCircle(*iter, CRAYON_THICKNESS/2);
            }
            
        } break;
            
        case ERASER_TOOL: {
            ofSetColor(0,0,0,0);
            ofDisableAlphaBlending();
            vector<ofVec2f> &curve = stroke.getCurve();
            for (vector<ofVec2f>::iterator iter=curve.begin();iter!=curve.end();iter++) {
                ofCircle(*iter, ERASER_THICKNESS/2);
            }
            ofEnableAlphaBlending();
           
        } break;
            
            
            
            
            
    }
}

void silentNature::draw(){    
	ofSetColor(255);
    
    
    ofPushMatrix();
    layout.draw();
    ofPopMatrix();
    
    
    ofPushMatrix();
    glMultMatrixf(cmat.getPtr())    ;
    ofSetColor(255);
#ifdef TARGET_OPENGLES
    canvasTex.draw(0, 0);
#else
    fbo.draw(0, 0);
#endif
    if (bDown) {
        switch (tool) {
            case BRUSH_TOOL:
            case CRAYON_TOOL:
                drawTool();
                break;
                
        }
    }
    ofPopMatrix();
    
    
    
   
    
}

//--------------------------------------------------------------
void silentNature::exit(){
    
    
}

//--------------------------------------------------------------
void silentNature::publish(){
    ofPixels pixels;
    
#ifdef TARGET_OPENGLES
    fbo.begin(canvasTex.getTextureData().textureID);
   glReadPixels(0, 0, 1024, 1024, GL_RGBA, GL_UNSIGNED_BYTE, pixels.getPixels());
    fbo.end();
#else
    fbo.readToPixels(pixels);
#endif
    
    
    string filename = "PHOTO_A_"+ofToString(counter)+".png";
    
    ofImage image;
    image.setFromPixels(pixels);
    image.saveImage(filename);
    
    ofxHttpForm form;
//	form.action = "http://localhost:8888/postImage.of";
	form.action = "http://192.168.10.6:8888/postImage.of";    
//    form.action = "http://192.168.10.5/upload/";
	form.method = OFX_HTTP_POST;
	form.addFormField("name", "fileupload1");
	form.addFile("file",filename);
	httpUtils.addForm(form);
    counter++;
    
}

//--------------------------------------------------------------
void silentNature::touchDown(ofTouchEventArgs &touch){
    
    ofVec2f pos = ofVec2f(touch.x,touch.y);
    
    vector<ofxSymbolInstance*> items = layout.hitTest(pos);
    
    for (vector<ofxSymbolInstance*>::iterator iter=items.begin(); iter!=items.end(); iter++) {
        
        if ((*iter)->type == SYMBOL_INSTANCE && (*iter)->bVisible == true) {
//            cout << iter->name << "\t";
            if ((*iter)->name.length() == 2) {
                if ((*iter)->name[0] == 'b' || (*iter)->name[0] == 'c') {
                    
                    this->tool = (*iter)->name[0] == 'b'  ? BRUSH_TOOL : CRAYON_TOOL;
                    
                    ofxSymbolInstance &sym = (*iter)->layers.front().instances.front();
                    if (sym.type == BITMAP_INSTANCE) {
                        cout << sym.bitmapItem->name << "\t" << sym.bitmapItem->href << endl;
                        ofImage &image = sym.bitmapItem->getImage();
                        this->color= image.getPixelsRef().getColor(image.getWidth()/2, image.getHeight()/2);
                         
                    } else {
                        this->color = ofColor(255);
                    }
                    
                    
                }
                
                
                
                if ((*iter)->name[0] == 'p') {
                    this->tool = CUTOUT_TOOL;
                    ofxSymbolInstance &sym = (*iter)->layers.front().instances.front();
                    if (sym.type == BITMAP_INSTANCE) {
                        cout << sym.bitmapItem->name << "\t" << sym.bitmapItem->href << endl;
                        paper = &sym.bitmapItem->getImage();
                    } else {
                        paper = NULL;
                    }
                    
                }
            }
            
            
            
            
            if ((*iter)->name == "eraser") {
                this->tool = ERASER_TOOL;
                
                break;
            }
            
            if ((*iter)->name == "canvas") {
                bDown = true;
                ofVec2f cpos = cmat.getInverse().preMult(ofVec3f(pos));
                
                switch (tool) {
                    case BRUSH_TOOL:
                        
                        stroke.setup(1);
                        touches.push_back(cpos);
                        break;
                    case CRAYON_TOOL:
                        stroke.setup(1);
                        touches.push_back(cpos);
                        break;
                        
                    case CUTOUT_TOOL:
                                               
#ifdef TARGET_OPENGLES
                        fbo.begin(canvasTex.getTextureData().textureID);
#else
                        fbo.begin();
#endif
                        if (paper!=NULL) {
                            ofPushMatrix();
                            ofTranslate(cpos);
                            
                            ofRotate(ofRandom(-180, 180));
                            float scale = ofRandom(0.7, 1.5);
                            ofScale(scale, scale);

                            ofTranslate(-0.5*ofVec2f(paper->getWidth(),paper->getHeight()));
                                
                                     
                            paper->draw(0,0);
                            ofPopMatrix();
                        }
                        fbo.end();
                            
                                                
                        break;
                        
                    case ERASER_TOOL:
                        
                        stroke.setup(1);
                        touches.push_back(cpos);
                        break;
                        
                    default:
                        break;
                }
                
            }
            
            if ((*iter)->name == "clean") {
#ifdef TARGET_OPENGLES
                fbo.begin(canvasTex.getTextureData().textureID);
#else
                fbo.begin();
#endif
                glClearColor(0,0,0, 0);
                glClear( GL_COLOR_BUFFER_BIT);
                fbo.end();
            }
            
            if ((*iter)->name == "publish") {
                publish();
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
                    case CRAYON_TOOL:
                        touches.push_back(cpos);
                        if (touches.size()==4) {
                            stroke.addPatch(vector<ofVec2f>(touches.begin(),touches.end()));
                            touches.pop_front();
                        }
                        
                        break;
                    case ERASER_TOOL:
                        touches.push_back(cpos);
                        if (touches.size()==4) {
                            stroke.addPatch(vector<ofVec2f>(touches.begin(),touches.end()));
                            touches.pop_front();
                            
#ifdef TARGET_OPENGLES
                            fbo.begin(canvasTex.getTextureData().textureID);
#else
                            fbo.begin();
#endif
                            drawTool();  
                            fbo.end();

                        }
                                                
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
            case CRAYON_TOOL:
                touches.clear();
#ifdef TARGET_OPENGLES
                fbo.begin(canvasTex.getTextureData().textureID);
#else
                fbo.begin();
#endif
                drawTool();
                fbo.end();
                
                break;
                
            case ERASER_TOOL:
                touches.clear();
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



void silentNature::updateTool() {
   
    
    
    layout.getChild("palette")->alphaMultiplier = 0.5;
    layout.getChild("crayons")->alphaMultiplier = 0.5;
    layout.getChild("paper")->alphaMultiplier = 0.5;
    layout.getChild("eraser")->alphaMultiplier = 0.5;

    
    
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

void silentNature::newResponse(ofxHttpResponse & response){
    cout << ofToString(response.status) + ": " + (string)response.responseBody << endl;
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