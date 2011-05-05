#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){	 

	// listen on the given port
	cout << "listening for osc messages on port " << RECEIVE_PORT << "\n";
	receiver.setup( RECEIVE_PORT );
	cout << "sending messages to port " << SEND_PORT << "\n";

	sender.setup( HOST, SEND_PORT );

	
	vidGrabber.listDevices();
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(CAM_WIDTH,CAM_HEIGHT);
	

	groups[0].name = "first_";
	groups[1].name = "second_";
	groups[2].name = "third_";
	groups[3].name = "fourth_";
	
	recordGroup = 0;
	
	preview = videoOutput(886,231,983,304,0);
	outputs[0] = videoOutput(294,150,774,510,0);
	outputs[1] = videoOutput(903,529,1007,607,1);
	outputs[2] = videoOutput(903,618,1007,696,2);
	outputs[3] = videoOutput(903,707,1007,785,3);

	
	saver.listCodecs();
	//saver.setCodecType(2); ZACH FIX see recording quicktime with sound in sync on of forum
	saver.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_NORMAL);
	

	mode = PLAY_MODE;
	bRecording = false;
	startTime = ofGetElapsedTimeMillis();
	
	lastFrameNumber = 0;
	
	text[0].loadImage("images/text_01.png");
	text[1].loadImage("images/text_02.png");
	text[2].loadImage("images/text_03.png");
	
	icon.loadImage("images/icon.png");
	//icon.setImageType(OF_IMAGE_COLOR_ALPHA);
	//icon.update();
	background.loadImage("images/background.png");
	overlay.loadImage("images/overlay.png");
	rec_overly.loadImage("images/rec_overly.png");
	bar.loadMovie("images/bar.mov");
	bar.setPaused(true);
	
	
	barFrame = 1;
	currentText = 0;
	
	x = 0;
	y = 0;
	
	folder = "videos/";
}


//--------------------------------------------------------------
void testApp::update(){
	
	ofBackground(100,100,100);
	
	
	
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );

		// check for mouse moved message
		
		if ( m.getAddress() == "/record" )
		{
			if (m.getNumArgs()==2) {
				recordGroup = m.getArgAsInt32( 0 ) % GROUPS_NUMBER;
				recordVideo = m.getArgAsInt32( 1 );
				mode=RECORD_MODE;
			}
		}
		else if ( m.getAddress() == "/play" )
		{
			mode=PLAY_MODE;
		}
		else if ( m.getAddress() == "/start" )
		{
			if (mode==RECORD_MODE) {
				startTime = ofGetElapsedTimeMillis();
				lastFrameNumber = 0;
			
				bRecording = true;
				saver.setup(CAM_WIDTH,CAM_HEIGHT,folder+groups[recordGroup].name+ofToString(recordVideo)+".mov");
				
				
				
			}
			
			
		}
		else if ( m.getAddress() == "/stop" )
		{
			if (mode==RECORD_MODE && bRecording) {
				mode = PLAY_MODE;
				bRecording = false;
				
				saver.finishMovie();
				groups[recordGroup].video.close();
				groups[recordGroup].video.loadMovie(folder+groups[recordGroup].name+ofToString(recordVideo)+".mov");
				groups[recordGroup].video.play();
				
			}
				
		}
		
		else if(m.getAddress() == "/trigger") {
			if (m.getNumArgs()>0)
			{
				int group = m.getArgAsInt32( 0 );
				if (m.getNumArgs()==2) {
					int count = m.getArgAsInt32( 1 );
					groups[group].video.close();
					groups[group].video.loadMovie(folder+groups[group].name+ofToString(count)+".mov");
					groups[group].video.play();
				}
				else {
					groups[group].video.setPaused(true);
					groups[group].video.setFrame(0);
					groups[group].video.setPaused(false);
				}
			}
		}
		else if(m.getAddress() == "/output") {
			if (m.getNumArgs()==2)
				outputs[m.getArgAsInt32( 0 )].group = m.getArgAsInt32( 1 );
		}
		else if(m.getAddress() == "/message") {
			if (m.getNumArgs()==1) 
				currentText = m.getArgAsInt32( 0 ) % 3;
		}
		else if(m.getAddress() == "/bar") {
			if (m.getNumArgs()==1) 
				bar.setFrame( m.getArgAsInt32( 0 ) % bar.nFrames);
		}
		
		else if(m.getAddress() == "/path") {
			if (m.getNumArgs()==1) 
				folder = m.getArgAsString(0);
		}
		
		
		
		else if ( m.getAddress() == "bang" )
		{
		}
	}

	int currentFrameNumber =  (ofGetElapsedTimeMillis() - startTime) *  FRAME_RATE / 1000.0;


	if (currentFrameNumber > lastFrameNumber) {
		lastFrameNumber = currentFrameNumber;
		vidGrabber.grabFrame();
		if (mode == RECORD_MODE && bRecording) 
			saver.addFrame(vidGrabber.getPixels(), 1.0f / (float)FRAME_RATE);
	}
	
	for (int i=0;i<GROUPS_NUMBER;i++)
		groups[i].video.update();

	bar.update();
	

}


//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(0xFFFFFF);
	background.draw(x,y);
	text[currentText].draw(x+209,y+545);
	
	vidGrabber.draw(x+preview.x,y+preview.y,preview.width,preview.height);
	
	if (mode == RECORD_MODE) {
		vidGrabber.draw(x+outputs[0].x,y+outputs[0].y,outputs[0].width,outputs[0].height);
	}
	
	//videoTexture.draw(20+CAM_WIDTH,20,CAM_WIDTH,CAM_HEIGHT);
	
	
	
	if (mode == PLAY_MODE) 
		groups[outputs[0].group].video.draw(x+outputs[0].x,y+outputs[0].y,outputs[0].width,outputs[0].height);
	
	for (int i=1;i<OUTPUTS_NUMBER;i++) {
		int group= outputs[i].group;
		groups[group].video.draw(x+outputs[i].x,y+outputs[i].y,outputs[i].width,outputs[i].height);
	}
	
	

	
	
	ofEnableAlphaBlending();
	icon.draw(x+678,y+465);
	overlay.draw(x+891,y+586);
	if (bRecording) {
		rec_overly.draw(x+outputs[0].x,y+outputs[0].y);
	}
	
	
	ofDisableAlphaBlending();
	
	bar.draw(x+213,y+510);
	
	/*
	ofSetColor(0x000000);
	ofDrawBitmapString( groups[recordGroup].name, 20, 20 );
	ofDrawBitmapString( mode == PLAY_MODE ? "play mode" : "record mode", 20, 40 );
	ofDrawBitmapString( bRecording ? "recording" : "idle", 20, 60 );
	ofDrawBitmapString( "frame: "+ofToString(lastFrameNumber), 20, 80 );
	 */

	
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	// in fullscreen mode, on a pc at least, the 
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...
	
	if (key == 's' || key == 'S'){
		vidGrabber.videoSettings();
	}
	
	if(key=='f')
		ofToggleFullscreen();
	
	{
		ofxOscMessage m;
		m.setAddress( "/key" );
		m.addIntArg( key );
		sender.sendMessage( m );
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
	this->x += x - px;
	this->y += y - py;
	px = x;
	py = y;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	px = x;
	py = y;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::resized(int w, int h){

}

