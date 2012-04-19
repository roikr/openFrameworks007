#include "testApp.h"


void copyPixels(int x,int y,ofImage &src, ofImage &dest) {
	
	for	(int j=0;j< dest.getHeight();j++)
		for (int i=0; i<dest.getWidth(); i++) {
			dest.setColor(i, j, src.getColor(x+i, y+j));
		}
	
}



//--------------------------------------------------------------
void testApp::setup(){
	src.setUseTexture(false);
	//src.loadImage(ofToDataPath("Leaf.JPG"));
	
	path = "images";
	
	size = 256;
	x = 0;
	y = 0;
	col = 0;
	row = 0;
	lastWidth = 0;
	lastHeight = 0;
	scale = 1;
	
	DIR.allowExt("jpg");
	DIR.listDir(path);
	fileCounter = 0;
	printf("num files: %i\n",DIR.numFiles());
	loadImage();
		
	
}

void testApp::loadImage() {
    src.clear();
	src.loadImage(ofToDataPath(DIR.getPath(fileCounter)));
	printf("path: %s\n",DIR.getPath(fileCounter).c_str());
	vector<string> name = ofSplitString(DIR.getName(fileCounter), ".");
	currentName = name[0];
	ofxDirList::createDirectory(currentName);
	printf("path: %s, currentName: %s\n",DIR.getPath(fileCounter).c_str(),currentName.c_str());
	
}

//--------------------------------------------------------------
void testApp::update(){
	
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
	} else {
        std::exit(0);
    }

	
	
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	dest.draw(0,0);
}

//--------------------------------------------------------------
void testApp::exit(){
	
	dest.clear();
    src.clear();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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