#include "testApp.h"

#define WARP_DIV	.125

//--------------------------------------------------------------
void testApp::setup(){
	trackers.loadTracks("pictureIphone.xml");
	player.loadMovie("pictureIphone.mov");
	image.loadImage("IMG_0103.JPG");
	player.play();
	cout << trackers.getNumTracks() << endl;
	start = ofGetElapsedTimeMillis();
	pos = 0;
	
	screenW = ofGetWidth();
	screenH = ofGetHeight();
	
	//pWarper.initWarp( image.getWidth(),image.getHeight(),image.getWidth()*WARP_DIV,image.getHeight()*WARP_DIV );
	pWarper.initWarp( screenW,screenH,screenW*WARP_DIV,screenH*WARP_DIV );
//	src[0] = ofPoint(0,0);
//	src[1] = ofPoint(screenW,0);
//	src[2] = ofPoint(screenW,screenH);
//	src[3] = ofPoint(0,screenH);
	
	fbo.allocate(screenW,screenH );
	fbo.clear();
	fbo.begin();
	ofScale(0.5, 0.5,1.0);
	image.draw(0, 0);
	fbo.end();
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	
	player.update();
	if (pos>player.getPosition()) {
		start = ofGetElapsedTimeMillis(); 
	}
	pos = player.getPosition();
	
	frame = (ofGetElapsedTimeMillis()-start)/40;
}

//--------------------------------------------------------------
void testApp::draw(){
	
	
	ofSetColor(255, 255, 255);
	player.draw(0, 0);
	ofSetColor(255, 0, 0);
	vector<pair<int,int> > quadrangle;
	int x,y;
	trackers.getPoint(0, frame, x, y);
	quadrangle.push_back(make_pair(x, y));
	trackers.getPoint(1, frame, x, y);
	quadrangle.push_back(make_pair(x, y));
	trackers.getPoint(3, frame, x, y);
	quadrangle.push_back(make_pair(x, y));
	trackers.getPoint(2, frame, x, y);
	quadrangle.push_back(make_pair(x, y));
	
	int i=0;
	float points[8];
	for (vector<pair<int,int> >::iterator iter=quadrangle.begin() ; iter!=quadrangle.end() ; iter++) {
		vector<pair<int,int> >::iterator iter2 = iter+1;
		if (iter2==quadrangle.end()) {
			iter2=quadrangle.begin();
		}
		
		ofLine(iter->first,iter->second,iter2->first,iter2->second);
		//dst[i]=ofPoint(iter->first,iter->second);
		points[2*i]=iter->first;
		points[2*i+1]=iter->second;
		i++;
	}
	
	ofSetColor(255, 255, 255);
	
	pWarper.recalculateWarp( points );
	fbo.drawWarped(0, 0,screenW, screenH,pWarper.u,pWarper.v,pWarper.num_x_sections,pWarper.num_y_sections);
	
		
	
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

