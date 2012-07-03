/*
 *  ofxPanZoom.cpp
 *  iPhoneEmptyExample
 *
 *  Created by Oriol Ferrer Mesià on 5/17/10.
 *  Copyright 2010 uri.cat. All rights reserved.
 *
 *  Modified by Roee Kremer on 12/6/12 to include animations
 */

#include "ofxPanZoom.h"

enum  {
	SLIDER_STATE_IDLE,
	SLIDER_STATE_PANNING,
    SLIDER_STATE_ZOOMING,
	SLIDER_STATE_ANIMATING
};



ofxPanZoom::ofxPanZoom(){
	
	zoom = 1.0f;	
	for (int i = 0; i < MAX_TOUCHES; i++){
		touching[i] = false;
	}
	
	minZoom = 0.1f;
	maxZoom = 10.0f;	
	zoomDiff = -1.0f;
	offset.x = offset.y = 0.0f;

	vFlip = true;
	viewportConstrained = false;
    
    state = SLIDER_STATE_IDLE;
}

void ofxPanZoom::setScreenSize(int x, int y){
	
	screenSize.x = x;
	screenSize.y = y;
	topLeft = screenToWorld( ofVec3f(0.0f, 0.0f) );
	bottomRight = screenToWorld( screenSize );
}


bool ofxPanZoom::isOnScreen( ofVec3f p ){	///gets point in gl coords, not screen coords
	
	if ( p.x > topLeft.x && p.x < bottomRight.x 
		 &&
		 p.y < topLeft.y && p.y > bottomRight.y 
		) return true;
	else
		return false;
}


void ofxPanZoom::apply(int customW, int customH){

	int ww = customW == 0 ? ofGetWidth() : customW;
	int hh = customH == 0 ? ofGetHeight() : customH;
	
	float w = ww * 0.5f / zoom;
	float h = hh * 0.5f / zoom;
    
    ofPushMatrix();

//	ofSetupScreenOrtho( ww, hh, (ofOrientation) ofxiPhoneGetOrientation(), true, -10.0f, 10.0f);
	glScalef( zoom, zoom, zoom);
	glTranslatef( offset.x + w + zoomOffset.x, offset.y + h + zoomOffset.y, 0.0f );	
	
	//recalc visible box
	topLeft = screenToWorld( ofVec3f() );
	bottomRight = screenToWorld( screenSize );
}


void ofxPanZoom::reset(){
    ofPopMatrix();
//	ofSetupScreen();	
}

void ofxPanZoom::lookAt( ofVec3f p ){
	offset.x = -p.x;
	offset.y = -p.y;
}

bool ofxPanZoom::fingerDown(){
	
	bool fingerDown = false;
	for (int i = 0; i < MAX_TOUCHES; i++) {
		if (touching[i] == true){
			fingerDown = true;
			break;
		}
	}
	return fingerDown;
}

ofVec3f ofxPanZoom::screenToWorld( ofVec3f p ){
	float f = 1.0f / zoom;
	p.x =  f * p.x - f * ofGetWidth() * 0.5f - offset.x ;
	p.y =  f * p.y - f * ofGetHeight() * 0.5f - offset.y ;
	return p;
}


void ofxPanZoom::drawDebug(){

	for (int i = 0; i < MAX_TOUCHES; i++){
		if (touching[i]) glColor4f(0, 1, 0, 1);
		else glColor4f(1, 0, 0, 1);
		float w = 8;
		ofRect( i * (w + 3), 3, w, w);
	}
	
	char msg[1000];
	sprintf(msg, " zoom: %.1f \n offset: %.1f, %.1f \n ", zoom, offset.x, offset.y);
	glColor4f(1, 1, 1, 1);
	ofDrawBitmapString(msg, 3.0f, 25.0f);
}


void ofxPanZoom::touchDown(ofTouchEventArgs &touch){

	lastTouch[touch.id].x = touch.x;
	lastTouch[touch.id].y = touch.y;

	//printf("####### touchDown %d (zoomdif: %f) %f %f \n", touch.id, zoomDiff , touch.x, touch.y);
	
	switch ( touch.numTouches ) {

		case 1:			
			break;

		case 2:			
			zoomDiff = lastTouch[0].distance( lastTouch[1] );
			break;

		default:
			break;
	}
	
	touching[touch.id] = true;
    
    pos = ofVec2f(touch.x,touch.y);
    velocity = ofVec2f(0,0);
    time = ofGetElapsedTimef();
}


void ofxPanZoom::touchMoved(ofTouchEventArgs &touch){
	
	ofVec3f p, now;
	float d;
	
	//printf("####### touchMoved %d (%.1f %.1f zoomdif: %f) \n", touch.id, touch.x, touch.y, zoomDiff);
	if (touching[touch.id] == false) return;
	
	switch ( touch.numTouches ) {

		case 1: {
			// 1 finger >> pan
			p = lastTouch[touch.id] - ofVec3f(touch.x,touch.y) ;
			offset = offset - p * (1.0f / zoom);
            
            
            state = SLIDER_STATE_PANNING;
            ofVec2f newPos = ofVec2f(touch.x,touch.y);
            float newTime = ofGetElapsedTimef();
            
            if (newTime-time>0) {
                velocity = (newPos-pos)/(newTime-time)/zoom;
            } else {
                velocity = ofVec2f(0,0);
            }
            
            pos = newPos;
            time = newTime;
            
//            setComponent(translate,getComponent(translate)+boundsFix(getComponent(touches.back().first-(touches.end()-2)->first)));
            
			applyConstrains();
		}   
        break;

		case 2:
			state = SLIDER_STATE_ZOOMING;
            velocity = ofVec2f(0,0);
			//pan with 2 fingers too
			p = 0.5 * ( lastTouch[touch.id] - ofVec3f(touch.x,touch.y) ); //0.5 to average both touch offsets
			offset = offset - p * (1.0 / zoom);	
			
			// 2 fingers >> zoom
			d = lastTouch[0].distance( lastTouch[1] );
			if (d > MIN_FINGER_DISTANCE ){

				//printf(" zoomDiff: %f  d:%f  > zoom: %f\n", zoomDiff, d, zoom);
				if ( zoomDiff > 0 ){
					zoom *= ( d / zoomDiff ) ;
					zoom = ofClamp( zoom, minZoom, maxZoom );
					float tx = ( lastTouch[0].x + lastTouch[1].x ) * 0.5f ;
					float ty = ( lastTouch[0].y + lastTouch[1].y ) * 0.5f ;
					tx -= ofGetWidth() * 0.5f;
					ty -= ofGetHeight() * 0.5f;
					//printf(" tx: %f   ty: %f  d / zoomDiff: %f \n", tx, ty, d / zoomDiff);
					if (zoom > minZoom && zoom < maxZoom){
						offset.x += tx * ( 1.0f - d / zoomDiff ) / zoom ;
						offset.y += ty * ( 1.0f - d / zoomDiff ) / zoom;
					}
					//printf(" zoom after %f \n", zoom);
				}

				zoomDiff = d;
				applyConstrains();
			}
			break;

		default:
			break;
	}

	lastTouch[touch.id].x = touch.x;
	lastTouch[touch.id].y = touch.y;
}


void ofxPanZoom::touchUp(ofTouchEventArgs &touch){

	//printf("####### touchUp %d (zoomdif: %f) \n", touch.id, zoomDiff);
	touching[touch.id] = false;
	lastTouch[touch.id].x = touch.x;
	lastTouch[touch.id].y = touch.y;
	if (touch.id == 0 || touch.id == 1)
		zoomDiff = -1.0f;
    
    
    if (touch.id==0 && velocity.length()>0) {
//        cout << velocity.length() << endl;
        state = SLIDER_STATE_ANIMATING;
        lastTime = ofGetElapsedTimef();
    } else {
        state = SLIDER_STATE_IDLE;
    }
}


void ofxPanZoom::touchDoubleTap(ofTouchEventArgs &touch){

}

void ofxPanZoom::setViewportConstrain(ofVec3f topLeftConstrain_, ofVec3f bottomRightConstrain_ ){
	viewportConstrained = true;
	topLeftConstrain = topLeftConstrain_;
	bottomRightConstrain = bottomRightConstrain_;
}

void ofxPanZoom::removeViewportConstrain(){
	viewportConstrained = false;	
}

void ofxPanZoom::applyConstrains(){
	
	if (viewportConstrained){
		float xx = screenSize.x * 0.5f * (1.0f /  zoom);
		float yy = screenSize.y * 0.5f * (1.0f /  zoom);

		if ( offset.x < topLeftConstrain.x + xx){ 
			offset.x = topLeftConstrain.x + xx;
			//printf("ox < topleft = %f\n", offset.x);
		}
		if( offset.y < topLeftConstrain.y + yy ){
			offset.y = topLeftConstrain.y + yy;
			//printf("oy < topleft = %f\n", offset.y);
		}
		
		if ( offset.x > bottomRightConstrain.x - xx){
			offset.x = bottomRightConstrain.x - xx;
			//printf("ox < bottomRight = %f\n", offset.x);
		}
		if (offset.y > bottomRightConstrain.y - yy){
			offset.y = bottomRightConstrain.y - yy;
			//printf("oy < bottomRight = %f\n", offset.y);
		}
	}
}

void ofxPanZoom::update() {
	
    
	if (state == SLIDER_STATE_ANIMATING) {
        if (velocity.length()>0.0001) {
            float t = ofGetElapsedTimef();
            offset+=velocity*(t-lastTime);
            velocity*=0.8;
            lastTime = t;
        } else {
            state = SLIDER_STATE_IDLE;
        }
		
	}	
    
}

bool ofxPanZoom::getIsAnimating() {
    return state == SLIDER_STATE_ANIMATING;
}