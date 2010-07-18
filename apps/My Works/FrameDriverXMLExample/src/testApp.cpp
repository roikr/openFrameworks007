#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);
	ofxXmlSettings videos;
	ofxXmlSettings lips;
	ofxXmlSettings player;
	
	string prefix = "VOC_PACIFIST";
	
	if (!videos.loadFile(prefix+"_VIDEOS.xml"))
		std::exit(0);
	
	if (!lips.loadFile(prefix+"_LIPS.xml"))
		std::exit(0);
	
	
	
	player.addTag("samples");
	player.pushTag("samples");
	
	int i;
	int j;
	int k;
	
	for (i=0; i<videos.getNumTags("sample"); i++) {
		string name = videos.getAttribute("sample","name","",i);
		int which = player.addTag("sample");
		videos.pushTag("sample", i);
		
		player.addAttribute("sample", "name", name, which);
		player.pushTag("sample", which);
		
		
		player.addTag("lipsync");
		
		for (j=0; j<lips.getNumTags("sample"); j++) {
			if (lips.getAttribute("sample", "name", "", j) == name) 
				break;
		}
		
		if (j<lips.getNumTags("sample")) {
			lips.pushTag("sample", j);
			lips.pushTag("lipsync");
			player.pushTag("lipsync");
			copyTag(lips, player);
			lips.popTag();
			player.popTag();
			lips.popTag();
		}
		
		player.addTag("video");
		player.addAttribute("video", "name", videos.getAttribute("video", "name", "",0),0);
		
		videos.pushTag("video");
		player.pushTag("video");
		copyTag(videos, player);
		videos.popTag();
		player.popTag();
		videos.popTag();
		player.popTag();
		
		
	}
	
	
	player.saveFile(prefix+".xml");
	std::exit(0);
}

void testApp::copyTag(ofxXmlSettings &src,ofxXmlSettings &dest) {
	for (int i=0;i<src.getNumTags("frame");i++) {
		int which = dest.addTag("frame");
		dest.addAttribute("frame", "number", src.getAttribute("frame", "number", 0, i), which);
		dest.addAttribute("frame", "use", src.getAttribute("frame", "use", 0, i), which);
	}
	
	
}

//--------------------------------------------------------------
void testApp::update(){
	

}

//--------------------------------------------------------------
void testApp::draw(){

	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

		
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



