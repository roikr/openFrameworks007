//
//  ofxDeepZoom.cpp
//  deepZoom
//
//  Created by Roee Kremer on 7/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxDeepZoom.h"



void ofxDeepZoom::setup(string filename,float width, float height,ofRectangle viewport) {
    this->width = width;
    this->height = height;
    this->viewport = viewport;
    tilesScale = 0.0f;
}

void ofxDeepZoom::start() {
    
}

void ofxDeepZoom::stop() {
    
}

void ofxDeepZoom::update(ofVec2f offset,float scale) {
    this->offset = offset;
    
    if (scale > 0 ) {
        this->scale = scale;
    }
    
    float factor = ceil(log(MIN(this->scale,1.0))/log(2));
	float newTilesScale = pow(2,factor);
    
    if (newTilesScale!=tilesScale) {
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
        
        tiles.clear();
        
        for (int i=0; i<numRows; i++) {
            for (int j=0; j<numCols ; j++) {
                tiles.push_back(tile(ofRectangle(j*rectSize, i*rectSize, rectSize, rectSize)));
            }
            tiles.push_back(tile(ofRectangle(numCols*rectSize, i*rectSize, widthRem, rectSize)));
        }
        
        for (int j=0; j<numCols ; j++) {
            tiles.push_back(tile(ofRectangle(j*rectSize, numRows*rectSize, rectSize, heightRem)));
        }
        
        tiles.push_back(tile(ofRectangle(numCols*rectSize,numRows*rectSize,widthRem,heightRem)));
        
        
        
    }
    
    rect.set(0.5*ofVec2f(width,height)+ofVec2f(viewport.x,viewport.y)/this->scale-offset/this->scale, viewport.width/this->scale,viewport.height/this->scale);
   
    //rect = ofRectangle((viewport.x-width/2/this->scale),(viewport.y-height/2/this->scale),);
    
//    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
    
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        
        iter->bInside = iter->rect.x+iter->rect.width>rect.x  && iter->rect.x<rect.x+rect.width  && iter->rect.y+iter->rect.height>rect.y && iter->rect.y<rect.y+rect.height;
    }
    
}

void ofxDeepZoom::draw() {
    
    ofPushStyle();

    
    
    ofPushMatrix();
    ofTranslate(offset);
    ofScale(scale, scale);
    ofTranslate(-width/2, -height/2);
    
    ofFill();
    
    ofEnableAlphaBlending();
    ofSetColor(50, 50, 50,50);
    ofRect(rect);
        
    
    
    
    
    
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        ofSetColor(iter->bInside ? 0 : 255, 0, iter->bInside ? 255 : 0, 100);
        ofRect(iter->rect);
    }
    
//    ofRect(0, 0, width, height);
    ofDisableAlphaBlending();
    ofNoFill();
    for (list<tile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
        ofSetHexColor(0xffffff);
        ofRect(iter->rect);
    }
    
    
    ofPopMatrix();
    
    ofPopStyle();
}