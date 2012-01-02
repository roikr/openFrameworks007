#include "testApp.h"


#define TILE_SIZE 256


//--------------------------------------------------------------
void testApp::setup(){
	path = "atsmona1";
	name = "atsmona1";
	width = 1946;
	height = 1908;
	scale = 0.125   ;
	pos = ofPoint(0,0);
		
	load();
}



//--------------------------------------------------------------
void testApp::update(){

/*
	if (fileCounter<DIR.numFiles()) {
	
		if (y<src.getHeight()) {
			int width = min(size,(int)src.getWidth()-x);
			int height = min(size,(int)src.getHeight()-y);
			if (width!=lastWidth || height!=lastHeight) {
				dest.clear();
				dest.allocate(width, height, OF_IMAGE_COLOR);
				lastWidth = width;
				lastHeight = height;
			}
			copyPixels(x, y, src, dest);
			dest.update();
			
			col = floor(x/size);
			row = floor(y/size);
			printf("scale: %i, row: %i, col: %i\n",int(scale*1000),row,col);
			char str[100];
//			sprintf(str, "png/%s_%i_%i_%i.png",name[0].c_str(),int(scale*1000),col,row);
			sprintf(str, "%s/%s_%i_%i_%i.jpg",currentName.c_str(),currentName.c_str(),int(scale*1000),col,row);
			dest.saveImage(string(str));

			//printf("x: %i, y: %i, width: %i, height %i\n",x,y,width,height);
			
			x+=width;
			if (x>=src.getWidth()) {
				y+=height;
				x=0;
			}
		} else {
			x = 0;
			y = 0;
			if (max(src.getWidth(),src.getHeight()) > 480 ) {
				src.resize(src.getWidth()/2, src.getHeight()/2);
				scale /= 2;
			} else {
				scale = 1;
				fileCounter++;
				if (fileCounter<DIR.numFiles()) {
					loadImage();
				}
			}

		}
	}

	
*/
	
	
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
			
			
			sprintf(str, "%s/%s_%i_%i_%i.jpg",path.c_str(),name.c_str(),(int)(1000*tilesScale),t.col,t.row);
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