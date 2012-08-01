//
//  ofxiDeepZoom.cpp
//  deepZoom
//
//  Created by Roee Kremer on 7/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxiDeepZoom.h"

#define TILE_SIZE 256



void ofxiDeepZoom::setup(string filename,string extension,float width, float height,ofRectangle viewport,EAGLSharegroup* sharegroup) {
    this->width = width;
    this->height = height;
    this->viewport = viewport;
    this->filename = filename;
    this->extension = extension;
    
    this->context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:sharegroup];
    tilesScale = 0.0f;
}

bool ofxiDeepZoom::isThreadRunning() {
    return bThreadRunning;
}

void ofxiDeepZoom::start() {
    bThreadRunning = true;
    dispatch_queue_t queue = dispatch_queue_create("Image Loader Queue", DISPATCH_QUEUE_SERIAL);
    dispatch_set_target_queue(queue, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0));
    dispatch_async(queue, ^(void) {
        threadedFunction();
        dispatch_async(dispatch_get_main_queue(), ^(void) {
			dispatch_release(queue);
		});
        
    });
                   
    
}

void ofxiDeepZoom::stop() {
    bThreadRunning = false;
}

string createTileName(string filename,string ext,int col,int row,float tilesScale) {
    char str[100];
    string path = filename;
    string name = filename;
    sprintf(str, "%s/%s_%i_%i_%i.%s",path.c_str(),name.c_str(),(int)(1000*tilesScale),col,row,ext.c_str());
    return str;
}

void ofxiDeepZoom::transform(ofVec2f offset,float scale) {
    this->offset = offset;
    
    if (scale > 0 ) {
        this->scale = scale;
    }
    
    float factor = ceil(log(MIN(this->scale,1.0))/log(2));
	float newTilesScale = pow(2,factor);
    
    if (newTilesScale!=tilesScale) {
        
        for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
            iter->bSwap = true;
        }
        
        tilesScale = newTilesScale;
        int rectSize = TILE_SIZE / tilesScale;
        
//        cout << "scale: " << this->scale << "\ttilesScale: " << tilesScale << endl;
//        cout << "factor: " << factor << "\trectSize: " << rectSize << endl;
        
        float newWidth = width*tilesScale;
        float newHeight = height*tilesScale;
        int numCols = floor(newWidth/TILE_SIZE);
        int numRows = floor(newHeight/TILE_SIZE);
        int widthRem = (newWidth-numCols*TILE_SIZE)/tilesScale;
        int heightRem = (newHeight-numRows*TILE_SIZE)/tilesScale;
        
        
        for (int i=0; i<numRows; i++) {
            for (int j=0; j<numCols ; j++) {
                tiles.push_back(tile(ofRectangle(j*rectSize, i*rectSize, rectSize, rectSize),createTileName(filename,extension, j, i, tilesScale)));
            }
            tiles.push_back(tile(ofRectangle(numCols*rectSize, i*rectSize, widthRem, rectSize),createTileName(filename,extension, numCols, i, tilesScale)));
        }
        
        for (int j=0; j<numCols ; j++) {
            tiles.push_back(tile(ofRectangle(j*rectSize, numRows*rectSize, rectSize, heightRem),createTileName(filename,extension, j, numRows, tilesScale)));
        }
        
        tiles.push_back(tile(ofRectangle(numCols*rectSize,numRows*rectSize,widthRem,heightRem),createTileName(filename,extension, numCols, numRows, tilesScale)));
        
        
        
    }
    
    rect.set(0.5*ofVec2f(width,height)+ofVec2f(viewport.x,viewport.y)/this->scale-offset/this->scale, viewport.width/this->scale,viewport.height/this->scale);
    
    rect.setFromCenter(rect.getCenter(), rect.width+2*TILE_SIZE*this->scale, rect.height+2*TILE_SIZE*this->scale);
   
    //rect = ofRectangle((viewport.x-width/2/this->scale),(viewport.y-height/2/this->scale),);
    
//    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
    
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        iter->bInside = iter->rect.x+iter->rect.width>rect.x  && iter->rect.x<rect.x+rect.width  && iter->rect.y+iter->rect.height>rect.y && iter->rect.y<rect.y+rect.height;
    }
    
}

/* we can use
if( lock() ) {
    ...
    unlock();
}
but we should not */
 

void ofxiDeepZoom::threadedFunction() {
    [EAGLContext setCurrentContext:context];
    while( isThreadRunning() != 0 ){
        
        for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
            
            switch (iter->state) {
                case TILE_STATE_INACTIVE:
                    if (iter->bInside) {
                        iter->state = TILE_STATE_LOAD;
                    }
                    break;
                case TILE_STATE_LOAD:
                    cout << iter->filename << endl;
                    iter->image.setUseTexture(false);
                    iter->image.loadImage(iter->filename);
                    glFlush();
                    
                    iter->state = TILE_STATE_ACTIVE;
                    break;
                case TILE_STATE_ACTIVE:
                    if (!iter->bInside) {
                        iter->state = TILE_STATE_UNLOAD;
                    }
                    break;
                case TILE_STATE_UNLOAD:
                    iter->image.clear();
                    iter->state = TILE_STATE_INACTIVE;
                    break;
                    
                    
                default:
                    break;
            }
            
            
            
        }
        
        ofSleepMillis(1 * 100);
    }
    
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        iter->image.clear();
    }
    cout << "tiles cleared" << endl;
    
}

bool isOverlapping(ofRectangle &a,ofRectangle &b) {
    return a.x+a.width>b.x  && a.x<b.x+b.width  && a.y+a.height>b.y && a.y<b.y+b.height;
}

bool isContaining(ofRectangle &a,ofRectangle &b) {
    return a.x<=b.x  && a.width>=b.width  && a.y<=b.y && a.height>=b.height;
}

bool ofxiDeepZoom::shouldSwap(tile &t) {
    vector<list<tile>::iterator> temp;
    
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        if (!iter->bSwap && isOverlapping(iter->rect, t.rect) && (!iter->bInside || iter->state==TILE_STATE_ACTIVE)){
            temp.push_back(iter);
        }
    }
    
    if (!temp.empty()) {
        if (temp.size()==1) {
            return isContaining(temp.front()->rect,t.rect);
        } else {
            float sum = 0;
            for (vector<list<tile>::iterator>::iterator iter=temp.begin();iter!=temp.end();iter++) {
                sum+=(*iter)->rect.width*(*iter)->rect.height;
            }
            return sum>=t.rect.width*t.rect.height;
        }
    }
    return false;
}


void ofxiDeepZoom::update() {
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        
        if (iter->bSwap) {
            switch (iter->state) {
                case TILE_STATE_INACTIVE:
                    iter = tiles.erase(iter);
                    break;
        
                case TILE_STATE_ACTIVE:
                    if (!iter->bInside || shouldSwap(*iter)) {
                        iter->state = TILE_STATE_UNLOAD;
                    }
                    break;
                    
        
                default:
                    break;
                    
            }
        }
    }
}

void ofxiDeepZoom::begin() {
    ofPushMatrix();
    ofTranslate(offset);
    ofScale(scale, scale);
}

void ofxiDeepZoom::end() {
    ofPopMatrix();
}


void ofxiDeepZoom::draw() {
    ofTranslate(-width/2, -height/2);
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        switch (iter->state) {
            case TILE_STATE_ACTIVE:
                iter->image.draw(iter->rect);
                break;
            default:
                break;
        }
    }
}

void ofxiDeepZoom::drawDebug() {
    
    ofPushStyle();
    
    
    ofTranslate(-width/2, -height/2);
    
    ofFill();
    
    ofEnableAlphaBlending();
    ofSetColor(50, 50, 50,50);
    ofRect(rect);
    
    
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        
        switch (iter->state) {
            case TILE_STATE_ACTIVE:
                ofSetColor(255, 255, 255,iter->bInside ? 150 : 50);
                iter->image.draw(iter->rect);
                break;
                
            default:
                ofSetColor(iter->bInside ? 0 : 255, 0, iter->bInside ? 255 : 0,50);
                ofRect(iter->rect);
                break;
        }
        
    }
    
    //    ofRect(0, 0, width, height);
    ofDisableAlphaBlending();
    ofNoFill();
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        ofSetHexColor(0xffffff);
        ofRect(iter->rect);
    }
    
    ofPopStyle();
}