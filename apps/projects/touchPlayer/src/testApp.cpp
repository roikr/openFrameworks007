#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	ofHideCursor();                     
	ofxXmlSettings xml;
	xml.loadFile("items.xml");
//	background.loadImage(xml.getAttribute("items", "background", ""));
	videoBackground.loadImage(xml.getAttribute("items", "videoBackground", ""));
	width = xml.getAttribute("items", "width", 0);
	height = xml.getAttribute("items", "height", 0);
	margin = xml.getAttribute("items", "margin", 0);
	
	xml.pushTag("items");
	
	back.loadImage(xml.getAttribute("back", "image", ""));
	backPos = ofPoint(xml.getAttribute("back", "x",0),xml.getAttribute("back", "y", 0));
	
	menuVideo.loadMovie(xml.getAttribute("menuVideo", "movie", ""));
	menuVideo.setLoopState(OF_LOOP_NORMAL);
	menuVideo.play();
	menuVideoPos = ofPoint(xml.getAttribute("menuVideoPos", "x",0),xml.getAttribute("menuVideoPos", "y", 0));
	
	for (int i=0; i<xml.getNumTags("item"); i++) {
		item it;
		it.movie.loadMovie(xml.getAttribute("item", "movie", "",i));
		it.movie.setLoopState(OF_LOOP_NONE);
		it.thumb.loadImage(xml.getAttribute("item", "thumb", "",i));
		it.pos = ofPoint(xml.getAttribute("item", "x",0,i),xml.getAttribute("item", "y", 0,i));
		items.push_back(it);
	}
	xml.popTag();
	
	citer = items.end();
}

//--------------------------------------------------------------
void testApp::update(){
	
	if	(citer != items.end()) {
		citer->movie.update();
		if (citer->movie.getIsMovieDone()) {
			citer->movie.stop();
			citer=items.end();
			menuVideo.play();
		}
	} else {
		menuVideo.update();
	}

}

//--------------------------------------------------------------
void testApp::draw(){
	if	(citer == items.end()) {
//		background.draw(0, 0);
		menuVideo.draw(menuVideoPos.x,menuVideoPos.y);
		ofEnableAlphaBlending();
		for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
			iter->thumb.draw(iter->pos.x,iter->pos.y);
			
		}
		
		ofDisableAlphaBlending();
		
	} else {
		videoBackground.draw(0, 0);
		float factor = (width-2*margin)/citer->movie.width;
		float y = (height-factor*citer->movie.height)/2;
		citer->movie.draw(margin,y,citer->movie.width*factor,citer->movie.height*factor);
		ofEnableAlphaBlending();
		back.draw(backPos.x, backPos.y);
		ofDisableAlphaBlending();
	}

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key='j') {
		if (citer!=items.end()) {
			citer->movie.setPaused(true);
			citer->movie.setPosition(0.95);
			citer->movie.setPaused(false);
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	if	(citer == items.end()) {
		for (vector<item>::iterator iter=items.begin(); iter!=items.end(); iter++) {
			if (x>iter->pos.x && x<iter->pos.x+iter->thumb.width && y>iter->pos.y && y<iter->pos.y+iter->thumb.height) {
				citer = iter;
				citer->movie.play();
				citer->movie.setPaused(true);
				citer->movie.setPosition(0);
				citer->movie.setPaused(false);
				
			}
		}
		
	} else {
		if (x>backPos.x && x<backPos.x+back.width && y>backPos.y && y<backPos.y+back.height) {
			citer->movie.stop();
			menuVideo.play();
			citer = items.end();
		}
	}
	
	
	

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){


}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

