#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);

	assert(xml.loadFile("downloader.xml"));
	
	xml.pushTag("downloader");
	background.loadImage(xml.getAttribute("background", "filename", ""));
	progressBar.loadImage(xml.getAttribute("progress_bar", "filename", ""));
	progressPos = ofPoint(xml.getAttribute("progress_bar", "x", 0),xml.getAttribute("progress_bar", "y", 0));
	titleOffset = ofPoint(xml.getAttribute("title_offset", "x", 0),xml.getAttribute("title_offset", "y", 0));
	normal.loadImage(xml.getAttribute("button", "normal", ""));
	highlighted.loadImage(xml.getAttribute("button", "highlighted", ""));
	selected.loadImage(xml.getAttribute("button", "selected", ""));
	
	for (int i=0; i<xml.getNumTags("bundle"); i++) {
		bundle b;
		b.path = xml.getAttribute("bundle", "path", "", i);
		b.title = xml.getAttribute("bundle", "title", "", i);
		b.pos = ofPoint(xml.getAttribute("bundle", "x", 0, i),xml.getAttribute("bundle", "y", 0, i));
		
		if (xml.attributeExists("bundle", "normal", i)) {
			b.normal = new ofImage;
			b.normal->loadImage(xml.getAttribute("bundle", "normal", "", i));
		} else {
			b.normal = &normal;
		}
		
		if (xml.attributeExists("bundle", "highlighted", i)) {
			b.highlighted = new ofImage;
			b.highlighted->loadImage(xml.getAttribute("bundle", "highlighted", "", i));
		} else {
			b.highlighted = &highlighted;
		}
		
		if (xml.attributeExists("bundle", "selected", i)) {
			b.selected = new ofImage;
			b.selected->loadImage(xml.getAttribute("bundle", "selected", "", i));
		} else {
			b.selected = &selected;
		}
		
		b.state = STATE_NORMAL;
		
		bundles.push_back(b);
	}
	
	xml.popTag();
	ttf.loadFont("mono.ttf", 20);
	progress= 0.5;
}

//--------------------------------------------------------------
void testApp::update(){
	//we change the background color here based on the values
	//affected by the mouse position
	ofBackground(255,255,255);

}

//--------------------------------------------------------------
void testApp::draw(){

	//---------
	//Lets draw the stroke as a continous line
	ofSetColor(0xFFFFFF);
	background.draw(0, 0);
	ofEnableAlphaBlending();
	progressBar.draw(progressPos.x, progressPos.y);
		
//
//	//instructions at the bottom
//	ttf.drawString("mouse drag changes background color and records stroke", 168, ofGetHeight() - 9);
	
	for (vector<bundle>::iterator iter=bundles.begin(); iter!=bundles.end(); iter++) {
		ofSetColor(0xFFFFFF);
		switch (iter->state) {
			case STATE_NORMAL:
				iter->normal->draw(iter->pos.x,iter->pos.y);
				break;
			case STATE_HIGHLIGHTED:
				iter->highlighted->draw(iter->pos.x,iter->pos.y);
				break;
			case STATE_SELECTED:
				iter->selected->draw(iter->pos.x,iter->pos.y);
				break;
			default:
				break;
		}
		
		ofSetColor(0x000000);
		ttf.drawString(iter->title, iter->pos.x+titleOffset.x, iter->pos.y+titleOffset.y);

	}
	ofDisableAlphaBlending();
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	for (vector<bundle>::iterator iter=bundles.begin(); iter!=bundles.end(); iter++) {
		if (iter->state!=STATE_SELECTED) {
			iter->state = (x >= iter->pos.x && x <= iter->pos.x+iter->normal->width && y >= iter->pos.y && y<=iter->pos.y+iter->normal->height) ? STATE_HIGHLIGHTED : STATE_NORMAL;
		}	
	}
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	vector<bundle>::iterator iter,siter;
	
	for (iter=bundles.begin(); iter!=bundles.end(); iter++) {
		if (x >= iter->pos.x && x <= iter->pos.x+iter->normal->width && y >= iter->pos.y && y<=iter->pos.y+iter->normal->height) {
			iter->state = STATE_SELECTED; 
			break;
		}
	}
	
	siter = iter;
	
	if (siter!=bundles.end()) {
		for (iter=bundles.begin(); iter!=bundles.end(); iter++) {
			if (iter!=siter) {
				iter->state = STATE_NORMAL;
			}
		}
	}
		
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
	
}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}



