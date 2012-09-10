//
//  silentNature.cpp
//  iSilentNature
//
//  Created by Roee Kremer on 7/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "silentNature.h"


#define BRUSH_THICKNESS 40.0
#define CRAYON_THICKNESS 5.0
#define ERASER_THICKNESS 40.0
#define APP_ORIENTATION  OF_ORIENTATION_DEFAULT // OF_ORIENTATION_90_LEFT

#ifdef TARGET_OPENGLES
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#endif

enum {
    STATE_DRAW,
    STATE_PUBLISH
};

//--------------------------------------------------------------
void silentNature::setup(){	
	
    //ofHideCursor();
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
    frame = doc.getSymbolItem("Frame")->createInstance("frame",mat);
    
    ofImage &image = doc.getBitmapItem("images/magenta")->getImage();
   
    tool = BRUSH_TOOL;
    color = image.getPixelsRef().getColor(image.getWidth()/2, image.getHeight()/2);
    paperNum = 1;
            
    canvas = layout.getChild("canvas");
    layout.getChildMat(canvas, cmat);
    ofxSymbolInstance *preview = frame.getChild("preview");
    frame.getChildMat(preview, previewMat);
    
    //cmat.translate(canvas->mat.getTranslation());
    
#ifdef TARGET_OPENGLES
    canvasTex.allocate(1024, 1024, GL_RGBA);
    //canvasTex.texData.bFlipTexture = true;
    fbo.setup(1024, 1024);
    
#else
    ofRectangle rect = canvas->getBoundingBox();
//    cout << rect.width << "\t" << rect.height << endl;
    fbo.allocate(rect.width, rect.height);
    chalk.setup(rect.width,rect.height);
    
    ofImage &pubcan=doc.getBitmapItem("images/publish_canvas.png")->getImage();
    publishFbo.allocate(pubcan.getWidth(), pubcan.getHeight());
#endif

    clear();
    
    bDown = false;
    
    //action_url = "http://localhost:8888/postImage.of";
	ofAddListener(httpUtils.newResponseEvent,this,&silentNature::newResponse);
	httpUtils.start();
    counter = 0;
    state = STATE_DRAW;

}




//--------------------------------------------------------------
void silentNature::update(){
    
    layout.update();
    
    if (ofGetElapsedTimeMillis()>publishTimer && state==STATE_PUBLISH) {
        state = STATE_DRAW;
        clear();
    }
    
    ofSetOrientation(APP_ORIENTATION); // just before we set viewport for draw
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
    
    switch (state) {
        case STATE_DRAW: 
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
            chalk.draw();
#endif
            if (bDown) {
                switch (tool) {
                    case BRUSH_TOOL:
                        drawTool();
                        break;
                        
                }
            }
            ofPopMatrix();
            
            break;
        case STATE_PUBLISH:
            ofBackgroundHex(0xd5d0ca);
            frame.draw();
            ofPushMatrix();
            glMultMatrixf(previewMat.getPtr());
            glMultMatrixf(publishMat.getPtr());
            fbo.draw(0, 0);
            ofPopMatrix();
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void silentNature::exit(){
    
    
}

ofRectangle getBoundaryBox(ofPixels &pixels) {
    ofRectangle rect;
    
    
    for (int i=0;i<pixels.getHeight();i++) {
        int j;
        for (j=0;j<pixels.getWidth();j++) {
            if (pixels.getColor(j, i).a!=0) {
                break;
            }
        }
        if (j<pixels.getWidth()) {
            rect.y = i;
            break;
        }
    }
    
    for (int i=0;i<pixels.getHeight();i++) {
        int j;
        for (j=0;j<pixels.getWidth();j++) {
            if (pixels.getColor(j, pixels.getHeight()-i-1).a!=0) {
                break;
            }
        }
        if (j<pixels.getWidth()) {
            rect.height = pixels.getHeight()-i-1-rect.y+1;
            break;
        }
    }
    
    for (int j=0;j<pixels.getWidth();j++) {
        int i;
        for (i=0;i<pixels.getHeight();i++) {
            if (pixels.getColor(j, i).a!=0) {
                break;
            }
        }
        if (i<pixels.getHeight()) {
            rect.x = j;
            break;
        }
    }
    
    for (int j=0;j<pixels.getWidth();j++) {
        int i;
        for (i=0;i<pixels.getHeight();i++) {
            if (pixels.getColor(pixels.getWidth()-j, i).a!=0) {
                break;
            }
        }
        if (i<pixels.getHeight()) {
            rect.width = pixels.getWidth()-j-1-rect.x+1;
            break;
        }
    }
    
    return rect;
}

//--------------------------------------------------------------
void silentNature::publish(){
    
    
    state = STATE_PUBLISH;
    publishTimer = ofGetElapsedTimeMillis() + 5000;
    ofPixels pixels;
    
    
#ifdef TARGET_OPENGLES
    fbo.begin(canvasTex.getTextureData().textureID);
   glReadPixels(0, 0, 1024, 1024, GL_RGBA, GL_UNSIGNED_BYTE, pixels.getPixels());
    fbo.end();
#else
    ofSetOrientation(OF_ORIENTATION_DEFAULT);
    fbo.readToPixels(pixels);
#endif
    
    ofRectangle rect = getBoundaryBox(pixels);
    cout <<  rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
    
    ofRectangle fboRect(0,0,fbo.getWidth(),fbo.getHeight());
    ofImage &pubcan=doc.getBitmapItem("images/publish_canvas.png")->getImage();
    
    float scale = min(pubcan.getWidth()/rect.width,pubcan.getHeight()/rect.height);

    publishMat.makeIdentityMatrix();
    publishMat.translate(0.5*ofVec2f(pubcan.getWidth(),pubcan.getHeight()));//+fboRect.getCenter());
    publishMat.preMultScale(ofVec3f(scale,scale,1.0));;
    publishMat.preMultTranslate(-rect.getCenter());
        
    
    ofImage image;
    image.setFromPixels(pixels);
#ifdef TARGET_OPENGLES
    
#else
    publishFbo.begin();
    pubcan.draw(0,0);
    ofPushMatrix();
    glMultMatrixf(publishMat.getPtr());
    image.draw(0,0);
    publishFbo.end();
    publishFbo.readToPixels(pixels);
#endif
    

    string filename = "images/PHOTO_A_"+ofToString(counter)+".png";
    
    ofImage publishImage;
    publishImage.setFromPixels(pixels);
    publishImage.saveImage(filename);
    
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

void silentNature::clear() {
#ifdef TARGET_OPENGLES
    fbo.begin(canvasTex.getTextureData().textureID);
#else
    fbo.begin();
#endif
    glClearColor(0,0,0, 0);
    glClear( GL_COLOR_BUFFER_BIT);
    fbo.end();

}

void silentNature::resetTools() {
    layer *ly = layout.getLayer("palette");
    for (vector<ofxSymbolInstance>::iterator iter = ly->frames[ly->currentFrame].instances.begin();iter!=ly->frames[ly->currentFrame].instances.end();iter++) {
        iter->gotoAndStop(0);
    }
    
    ly = layout.getLayer("crayons");
    for (vector<ofxSymbolInstance>::iterator iter = ly->frames[ly->currentFrame].instances.begin();iter!=ly->frames[ly->currentFrame].instances.end();iter++) {
        iter->gotoAndStop(0);
    }
    
    ly = layout.getLayer("papers");
    for (vector<ofxSymbolInstance>::iterator iter = ly->frames[ly->currentFrame].instances.begin();iter!=ly->frames[ly->currentFrame].instances.end();iter++) {
        iter->gotoAndStop(0);
    }
    
}

//--------------------------------------------------------------
void silentNature::touchDown(ofTouchEventArgs &touch){
    
    if (state==STATE_PUBLISH) {
        return;
    }
    
    ofVec2f pos = ofVec2f(touch.x,touch.y);
    
    vector<ofxSymbolInstance*> items;
    
    if (layout.hitLayer(layout.getLayer("palette"), pos, items)) {
        resetTools();
        items.front()->play();
        this->tool = BRUSH_TOOL ;
        
        ofxSymbolInstance &sym = items.front()->layers.front().frames.front().instances.front();
        if (sym.type == BITMAP_INSTANCE) {
            cout << sym.bitmapItem->name << "\t" << sym.bitmapItem->href << endl;
            ofImage &image = sym.bitmapItem->getImage();
            this->color= image.getPixelsRef().getColor(image.getWidth()/2, image.getHeight()/2);
            
        } else {
            this->color = ofColor(255);
        }
    }
    
    items.clear();
    if (layout.hitLayer(layout.getLayer("crayons"), pos, items)) {
        resetTools();
        items.front()->play();
        this->tool = CRAYON_TOOL ;
        
        ofxSymbolInstance &sym = items.front()->layers.front().frames.front().instances.front();
        if (sym.type == BITMAP_INSTANCE) {
            cout << sym.bitmapItem->name << "\t" << sym.bitmapItem->href << endl;
            ofImage &image = sym.bitmapItem->getImage();
            chalk.setColor(image.getPixelsRef().getColor(image.getWidth()/2, 5)); // get the crayon tip color
            
        } else {
            this->color = ofColor(255);
        }
    }
    
    items.clear();
    layer *papers = layout.getLayer("papers");
    if (layout.hitLayer(papers, pos, items)) {
        resetTools();
//        for (vector<ofxSymbolInstance *>::iterator iter = items.begin();iter!=items.end();iter++) {
//            cout << (*iter)->name << "\t";
//        }
//        cout << endl;
        

        paperNum = items.front()->name[5]-48;
//        papers->frames.front().instances.front().gotoAndStop(paperNum); // "paper1"...
        this->tool = CUTOUT_TOOL ;

    }
    
    items.clear();
    if (layout.hitLayer(layout.getLayer("eraser"), pos, items)) {
        resetTools();
        items.front()->play();
        this->tool = ERASER_TOOL;
    }
    
    items.clear();
    if (layout.hitLayer(layout.getLayer("canvas"), pos, items)) {
        
        bDown = true;
        ofVec2f cpos = cmat.getInverse().preMult(ofVec3f(pos));
        
        switch (tool) {
            case BRUSH_TOOL:
                
                stroke.clear(1);
                stroke.addTouch(cpos);
                
                break;
            case CRAYON_TOOL:
                chalk.clear();
                chalk.touch(cpos);
                break;
                
            case CUTOUT_TOOL: {
                
                
                
#ifdef TARGET_OPENGLES
                fbo.begin(canvasTex.getTextureData().textureID);
#else
                ofSetOrientation(OF_ORIENTATION_DEFAULT);
                fbo.begin();
#endif
                
                ofPushMatrix();
                ofTranslate(cpos);
                
                ofRotate(ofRandom(-180, 180));
                float scale = ofRandom(0.7, 1.5);
                ofScale(scale, scale);
                
                ofImage &paper = doc.getBitmapItem("papers_folder/paper"+ofToString(paperNum)+(paperNum == 1 ? "_"+ofToString(rand()%5):"")+".png")->getImage();
                ofTranslate(-0.5*ofVec2f(paper.getWidth(),paper.getHeight()));
                
                paper.draw(0,0);
                ofPopMatrix();
                
                fbo.end();
                
                
            } break;
                
            case ERASER_TOOL:
                
                stroke.clear(7);
               
                break;
                
            default:
                break;
        }
            
    }
                    
            
    items.clear();
    if (layout.hitLayer(layout.getLayer("clean"), pos, items)) {
        clear();
    }       
            
            
    items.clear();
    if (layout.hitLayer(layout.getLayer("publish"), pos, items)) {       
        publish();
    }

            
    
    
    cout << endl;
    
}

//--------------------------------------------------------------
void silentNature::touchMoved(ofTouchEventArgs &touch){
    
    if (state==STATE_PUBLISH) {
        return;
    }
    
    ofVec2f pos = ofVec2f(touch.x,touch.y);
    
    vector<ofxSymbolInstance*> items;
    
     if (layout.hitLayer(layout.getLayer("canvas"), pos, items)) {
         ofVec2f cpos = cmat.getInverse().preMult(ofVec3f(pos));
         
         switch (tool) {
             case BRUSH_TOOL:
                 stroke.addTouch(cpos);
                 
                 
                 break;
            case CRAYON_TOOL:
                 chalk.touch(cpos);
                 break;
             case ERASER_TOOL:
                 stroke.addTouch(cpos);                     
#ifdef TARGET_OPENGLES
                 fbo.begin(canvasTex.getTextureData().textureID);
#else
                 ofSetOrientation(OF_ORIENTATION_DEFAULT);
                 fbo.begin();
#endif
                 drawTool();  
                 fbo.end();
                     
                 
                 
                 break;
                 
             default:
                 break;
         }

     }

    
    

}

//--------------------------------------------------------------
void silentNature::touchUp(ofTouchEventArgs &touch){
    
    if (state==STATE_PUBLISH) {
        return;
    }
    
    if (bDown) {
        switch (tool) {
            case BRUSH_TOOL:
            
                
#ifdef TARGET_OPENGLES
                fbo.begin(canvasTex.getTextureData().textureID);
#else
                ofSetOrientation(OF_ORIENTATION_DEFAULT);
                fbo.begin();
#endif
                drawTool();
                fbo.end();
                
                break;
                
            case CRAYON_TOOL:
                chalk.touch(cmat.getInverse().preMult(ofVec3f(touch.x,touch.y,0)));
                break;
            case ERASER_TOOL:
               
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