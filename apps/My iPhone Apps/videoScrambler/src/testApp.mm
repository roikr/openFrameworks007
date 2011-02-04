#include "testApp.h"
#include "ofMainExt.h"

#include "ofxXmlSettings.h"





//--------------------------------------------------------------
void testApp::setup(){	
	//ofDisableDataPath();
	// register touch events
	ofxRegisterMultitouch(this);
	
	//ofxCopyFile(ofToResourcesPath("IMG_0121.MOV"), ofToDataPath("IMG_0121.MOV"));
	//ofxCopyFile(ofToResourcesPath("can.m4v"), ofToDataPath("can.m4v"));
	
	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
 
	
	ofBackground(0,0,0);
		
	
	ofSetFrameRate(24);
	
	int bufferSize = 256;
	int sampleRate = 44100;
	
	ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 2);	

	streamer.setup(ofToResourcesPath("can.m4v"));
	//streamer.setup("http:////www.roikr.com//can.m4v");
	//streamer.setup(ofToDataPath("IMG_0121.MOV"));
	bStarted = false;
	
	
}

void testApp::fillVideoTile(videoTile &t,ofRectangle rect,ofRectangle videoRect) {
	float aspectRatio = videoRect.width/videoRect.height;
	
	t.rect = rect;
	t.tex.x = (t.rect.x-videoRect.x)/videoRect.height/aspectRatio;
	t.tex.y = (t.rect.y-videoRect.y)/videoRect.height;
	t.tex.width = t.rect.width/videoRect.width;
	t.tex.height = t.rect.height/videoRect.height;
		
}


void testApp::setupTiles() {
	
	float aspectRatio = streamer.getWidth()/streamer.getHeight();
	
	videoRect.width = ofGetHeight()*aspectRatio;
	videoRect.height = ofGetHeight();
	videoRect.x = (ofGetWidth() - videoRect.width)/2;
	videoRect.y = 0;
//	
//	tile.tex.x = 0;
//	tile.tex.y = 0;
//	tile.tex.width = 1;
//	tile.tex.height = 1;
	
	ofRectangle margin;
	margin.x = videoRect.x;
	margin.y = 0;
	margin.width = (videoRect.width-videoRect.height)/2;
	margin.height = videoRect.height;
	fillVideoTile(leftMargin, margin, videoRect);
	margin.x+=videoRect.width-margin.width;
	fillVideoTile(rightMargin, margin, videoRect);
	
	tilesPerRow = 4;
	offset = leftMargin.rect.x+leftMargin.rect.width;
	tileSize = ofGetHeight()/tilesPerRow;
		
	for (int i=0; i<tilesPerRow*tilesPerRow-1	; i++) {
		
		videoTile t;
		ofRectangle rect;
		rect.x = (i%tilesPerRow)*tileSize+leftMargin.rect.x+leftMargin.rect.width;
		rect.y = (int)(i/tilesPerRow)*tileSize;
		rect.width = tileSize;
		rect.height = tileSize;
		
		fillVideoTile(t, rect, videoRect);
				
		tiles.push_back(t);
	}
	
	
	
	current=tiles.end();
		
	
	
	
	//tile.tex.x = (float)pos.x/(float)ofGetWidth();
	//tile.tex.y = (float)pos.y/(float)ofGetHeight();
}


//--------------------------------------------------------------
void testApp::update(){
	//image.update();
	streamer.update();
	
	if (!bStarted && streamer.getIsStreaming()) {
		bStarted = true;
		setupTiles();
	}
	
	
	
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	//ofPushMatrix();
	//ofTranslate(pos.x, pos.y, 0);
	//ofScale(0.5, 0.5, 1);
	
	ofSetColor(255, 255, 255);
	
	streamer.draw(leftMargin.rect,leftMargin.tex);
	streamer.draw(rightMargin.rect,rightMargin.tex);
	
	
	for(vector<videoTile>::iterator iter = tiles.begin();iter!=tiles.end();iter++) {
		streamer.draw(iter->rect,iter->tex);
	}
	//streamer.draw(videoRect,tile.tex);
	//ofPopMatrix();
	
	
	
	
	ofSetColor(255, 0, 0);
	
	float sfps = streamer.getFrameRate();
	std::ostringstream ss;
	
	ss << sfps << "\n" << ofGetFrameRate() << "\n";
	
//	for (int i=0; i<4; i++) {
//		ss << rowCounters[i] << " " << columnCounters[i] << "\n";
//	}
	
	
	
	
//	if (current!=tiles.end()) {
//		ss << "touch" << "\n";
//	}
	
	ofDrawBitmapString(ss.str(), 0, 20);
	
	//ofSetFrameRate(sfps);
	
		
	
	//ofDisableAlphaBlending();
	
	//--------------------------- rectangles
//	ofFill();
//	for (int i = 0; i < 200; i++){
//		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
//		ofRect(ofRandom(250,350),ofRandom(250,350),ofRandom(10,20),ofRandom(10,20));
//	}
//	ofSetColor(0x000000);
//	ofDrawBitmapString("rectangles", 275,500);
//
//
//	pincher.draw();

}



void testApp::touchDown(int x, int y, int id) {
	
	if (!bStarted) {
		return;
	}
	
	down = ofPoint(x,y);
	
		
	for (current=tiles.begin(); current!=tiles.end(); current++) {
		if (x>=current->rect.x && x<current->rect.x+current->rect.width && y>=current->rect.y && y<current->rect.y+current->rect.height) {
			break;
		}
	}		
		
	
//	if (current!=tiles.end()) {
//		cout << distance(tiles.begin(), current) << ": " <<current->rect.x << " " << current->rect.y << endl;
//	}
				
	

}

bool testApp::getIsInside(ofRectangle &rect,ofPoint p) {
	
	return p.x>rect.x && p.x<rect.x+rect.width && p.y>rect.y && p.y<rect.y+rect.width;
	
	
}

bool testApp::getIsIntersecting(ofRectangle &r1,ofRectangle &r2) {
	return getIsInside(r1, ofPoint(r2.x,r2.y)) || getIsInside(r1, ofPoint(r2.x,r2.y+r2.height)) ||
	getIsInside(r1, ofPoint(r2.x+r2.width,r2.y)) || getIsInside(r1, ofPoint(r2.x+r2.width,r2.y+r2.height)) ||
	getIsInside(r1, ofPoint(r2.x+r2.width/2,r2.y)) || getIsInside(r1, ofPoint(r2.x+r2.width/2,r2.y+r2.height)) ||
	getIsInside(r1, ofPoint(r2.x,r2.y+r2.height/2)) || getIsInside(r1, ofPoint(r2.x+r2.width,r2.y+r2.height/2));
}


void testApp::touchMoved(int x, int y, int id) {
	
	if (!bStarted) {
		return;
	}
	ofPoint diff = ofPoint(x,y)-down;
		
	
	if (current!=tiles.end() ) {
		
		ofPoint pos(current->rect.x/tileSize,current->rect.y/tileSize);
		
		
		if (pos.y==(int)pos.y) {
			current->rect.x+=diff.x;
			
			for (vector<videoTile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
				
				if (current==iter ) {
					continue;
				}
				
				if (getIsIntersecting(current->rect, iter->rect)) {
					//printf("x: %i and %i are intersecting\n",distance(tiles.begin(), current),distance(tiles.begin(), iter));
					
			
					if (current->rect.x+current->rect.width > iter->rect.x && current->rect.x+current->rect.width < iter->rect.x+iter->rect.width) {
						current->rect.x=iter->rect.x-current->rect.width;
					}
					if (current->rect.x > iter->rect.x  && current->rect.x < iter->rect.x+iter->rect.width) {
						current->rect.x=iter->rect.x+iter->rect.width;
						
					}
				}
			} 
			current->rect.x=ofClamp(current->rect.x, offset, tileSize*(tilesPerRow-1)+offset);
		} 
		
		pos= ofPoint(current->rect.x/tileSize,current->rect.y/tileSize);
		
		if (pos.x==(int)pos.x ) {
		
		
			current->rect.y+=diff.y;
			
				
			for (vector<videoTile>::iterator iter=tiles.begin(); iter!=tiles.end(); iter++) {
				
				if (current==iter ) {
					continue;
				}
				
				if (getIsIntersecting(current->rect, iter->rect)) {
				
					//printf("y: %i and %i are intersecting\n",distance(tiles.begin(), current),distance(tiles.begin(), iter));
					
					if (current->rect.y+current->rect.height > iter->rect.y && current->rect.y+current->rect.height < iter->rect.y+iter->rect.height) {
						current->rect.y=iter->rect.y-current->rect.height;
					}
					
					if (current->rect.y > iter->rect.y  && current->rect.y < iter->rect.y+iter->rect.height) {
						current->rect.y=iter->rect.y+iter->rect.height;
						
					}
				}
			} 
			
			current->rect.y=ofClamp(current->rect.y, 0, tileSize*(tilesPerRow-1));
			
		}
	}
	
	down = ofPoint(x,y);
		
}

void testApp::touchUp(int x, int y, int id) {
	if (!bStarted) {
		return;
	}
	current = tiles.end();
	
		
}

void testApp::touchDoubleTap(int x, int y, int id) {

}


void testApp::audioRequested( float * output, int bufferSize, int nChannels ) {
	
	streamer.audioRequested(output, bufferSize, nChannels);

}



