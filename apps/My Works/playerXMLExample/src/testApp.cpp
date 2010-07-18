#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);
	ofxXmlSettings textures;
	ofxXmlSettings player;
	
	if (!textures.loadFile("textures.xml"))
		std::exit(0);
	
	string name = "BB";
	
	
	textures.pushTag("actor");
	
	player.addTag("video_set");
	player.pushTag("video_set");
	player.addTag("actor");
	player.addTag("actor");
	player.addAttribute("actor","path","VOC_"+name,0);
	player.addAttribute("actor","path","VOC_"+name,1);
	player.addAttribute("actor","lips",1,1);
	
		
	for (int i=0;i<textures.getNumTags("sequence");i++) {
		string prefix = textures.getAttribute("sequence","prefix","",i);
		
		vector<string> split = ofSplitString(prefix, "_");
		if (split[1] == name ) {
			cout << prefix ;
			if (split.back() == "LIPS") 
				player.pushTag("actor", 1);
			else
				player.pushTag("actor", 0);
			
			copyTag(textures, player, i);
			player.popTag();
			
			cout << endl;
		}
		//textures.pushTag("sequence");

	}
	player.saveFile("VOC_"+name+".xml");
	std::exit(0);
}

void testApp::copyTag(ofxXmlSettings &src,ofxXmlSettings &dest,int which) {
	int i = dest.addTag("sequence");
	dest.addAttribute("sequence", "prefix", src.getAttribute("sequence", "prefix", "", which),i);
	dest.addAttribute("sequence", "width", src.getAttribute("sequence", "width", 0, which),i);
	dest.addAttribute("sequence", "height", src.getAttribute("sequence", "height", 0, which),i);
	dest.addAttribute("sequence", "frames", src.getAttribute("sequence", "frames", 0, which),i);
	src.pushTag("sequence", which);
	dest.pushTag("sequence", i);
	for (int j=0; j<src.getNumTags("textures"); j++) {
		int k = dest.addTag("textures");
		dest.addAttribute("textures", "size", src.getAttribute("textures", "size", 0, j), k);
		dest.addAttribute("textures", "number", src.getAttribute("textures", "number", 0, j), k);
	}
	src.popTag();
	dest.popTag();
	
	
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



