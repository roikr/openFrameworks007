#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
	ofBackground(127,127,127);
	
	ofAddListener(gkReceiveDataEvent, this,&testApp::gkReceiveData);
	
	game.startPicker();
	
	int textureWidth = 64;
	int textureHeight = 64;
	texture.allocate(textureWidth, textureHeight, GL_RGBA);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	texture.draw(0, 0,320,320);
}

//--------------------------------------------------------------
void testApp::exit(){

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	
	string str = "hello";
	ofBuffer buf(str.c_str(),str.length()+1);
	
	game.sendData(buf);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){

}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}


//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs& args){

}

void testApp::gkReceiveData(gkReceiveDataEventArgs &args) {
	cout << args.data.getText() << endl;
	texture.loadData(reinterpret_cast<unsigned char*> ( args.data.getBinaryBuffer()),texture.getWidth(),texture.getHeight(),GL_RGBA);
	
}

