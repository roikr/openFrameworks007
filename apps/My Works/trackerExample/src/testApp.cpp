#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	trackers.loadTracks("pictureIphone.xml");
	player.loadMovie("pictureIphone.mov");
	player.play();
	cout << trackers.getNumTracks() << endl;
	start = ofGetElapsedTimeMillis();
	pos = 0;
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
	
	
	for (vector<pair<int,int> >::iterator iter=quadrangle.begin() ; iter!=quadrangle.end() ; iter++) {
		vector<pair<int,int> >::iterator iter2 = iter+1;
		if (iter2==quadrangle.end()) {
			iter2=quadrangle.begin();
		}
		
		ofLine(iter->first,iter->second,iter2->first,iter2->second);
		
	}
	
	
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

