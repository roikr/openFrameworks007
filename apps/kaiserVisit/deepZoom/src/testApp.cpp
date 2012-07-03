#include "testApp.h"


#define TILE_SIZE 256


//--------------------------------------------------------------
void testApp::setup(){
	path = "IMAGE_1";
	name = "IMAGE_1";
    ext = "png";
	width = 5053;
	height = 3517;
	scale = 0.125   ;
	pos = ofPoint(0,0);
		
	load();
}



//--------------------------------------------------------------
void testApp::update(){

	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofPushMatrix();
	ofScale(scale/tilesScale, scale/tilesScale, 1.0f);
	ofTranslate(pos.x, pos.y);
	
	for (vector<tile>::iterator iter = tiles.begin(); iter!=tiles.end(); iter++) {
		ofPushMatrix();
		ofTranslate(TILE_SIZE*iter->col, TILE_SIZE*iter->row);
		iter->image.draw(0,0);
		ofPopMatrix();
	}
	ofPopMatrix();
	
}

void testApp::load() {
	tiles.clear();
	
	float factor = ceil(log(scale)/log(2));
	tilesScale = pow(2,factor);
	printf("factor: %f, tilesScale: %f\n",factor,tilesScale);
	
	char str[100];
	for (int y=0; y*TILE_SIZE<height*tilesScale; y++) {
		for (int x=0; x*TILE_SIZE<width*tilesScale; x++) {
			tile t;
			t.col = x;
			t.row = y;
			
			
			sprintf(str, "%s/%s_%i_%i_%i.%s",path.c_str(),name.c_str(),(int)(1000*tilesScale),t.col,t.row,ext.c_str());
			printf("%s\n",str);
			t.image.loadImage(string(str));
			tiles.push_back(t);
		}
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	switch (key) {
		case '+':
		case '-': {
			float newScale = scale;
			
			switch (key) {
				case '+':
					newScale+=0.05;
					break;
				case '-':
					newScale-=0.05;
					break;
					
				default:
					break;
			}
			
			
			int scalePower = floor(log(scale)/log(2));
			int newScalePower = floor(log(newScale)/log(2));
			
			bool bTilesScaleChanged =  scalePower != newScalePower && newScalePower>=-3; 
			printf("scale: %f, newScale: %f, update: %i\n",scale,newScale,bTilesScaleChanged);
			 scale = newScale;
			
			if (bTilesScaleChanged) {
				load();
			}
			
			
			
			
		}  break;

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
	ofPoint curPos = ofPoint(x,y);
	pos += curPos-downPos;
	downPos = curPos;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	downPos = ofPoint(x,y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	printf("width: %d, height: %d\n",ofGetWidth(),ofGetHeight());
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}