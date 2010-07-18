

#include "testApp.h"
#include "ofxXmlSettings.h"


#include "ofxRKUtils.h"
#include "Player.h"


#define kNumberPlayers 3

//--------------------------------------------------------------
void testApp::setup(){	
	ofSetFrameRate(60);
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	// touch events will be sent to myTouchListener
	ofxMultiTouch.addListener(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	// initialize all of the Thing particles
	//for(int i=0; i<NUM_POINTS; i++) balls[i].init();
	
	
	string inputfn = "ohYeahPlastic.xml"; 
	string outputfn = "ohYeahPlastic.wav";
	
	
	cout << endl;
	cout << "input file: " << inputfn << endl;
	cout << "output file: " << outputfn << endl;
	
	
	ofxXmlSettings xml;
	bool loaded = xml.loadFile("dead.xml");
	
	assert(loaded);
	if (!loaded)
		std::exit(0);
	
	Player *player;
	
	xml.pushTag("dead");
	
	
	xml.pushTag("guitar",0);
	player=new Player();
	player->setup(&xml,0,&mix,&thread);
	players.push_back(player);
	xml.popTag();
	
	
	xml.pushTag("vocal",0);
	player=new Player();
	player->setup(&xml,0,&mix,&thread);
	players.push_back(player);
	xml.popTag();
	 
	
	xml.pushTag("drum",0);
	player=new Player();
	player->setup(&xml,0,&mix,&thread);
	players.push_back(player);	
	xml.popTag();

	xml.popTag();
	
	
	output.SetOutput(2, &mix);
	
	//thread.AddObj(&mix);
	thread.AddObj(&mix);
	thread.AddObj(&output,SNDIO_OUT);
	
	
	ofxXmlSettings midiXml;
	
	//if (bUseArgs) 
	//	ofDisableDataPath();
	
	midiXml.loadFile(inputfn);
	
	//ofEnableDataPath();
	
	midiXml.pushTag("MIDIFile");
	ticksPerBeat = midiXml.getValue("TicksPerBeat", 96);
	for (int i=0; i<kNumberPlayers; i++) {
		midiXml.pushTag("Track", i);
		players[i]->setMidiTrack(&midiXml);
		midiXml.popTag();
	}
	midiXml.popTag();
	
	

	//string filename = ofToDocumentsPath(outputfn);
	
		
	thread.ProcOn();
	startTime = ofGetElapsedTimeMillis();
	startThread(false, false);   // non blocking, verbose
}

void testApp::threadedFunction(){
	
	while( isThreadRunning() != 0 ){
		
		
		int currentTick = (ofGetElapsedTimeMillis() - startTime)  *  ticksPerBeat * 60 / (1000 * 60);
		for (int i=0; i<kNumberPlayers; i++) 
			players[i]->update(currentTick);
				//int tick = currentFrame * DEF_VECSIZE * ticksPerBeat * (120 / 60) / (int) DEF_SR;
		//cout << "tick: " << tick << endl;
		/*
		for (int i=0; i<kNumberPlayers; i++) {
			bExit = bExit && players[i]->getIsSongDone();
			if (!bExit)
				break;
		}
		 */
		
		
			
			
		
		
		
		/*
		//int currentTick = (ofGetElapsedTimeMillis() - startTime)  *  midi.getTicksPerBeat() * 120 / 1000 / 60;
		int currentTick = (ofGetElapsedTimeMillis() - startTime)  *  midi.getTicksPerBeat() / 500;
		event e;
		
		if (!midi.getIsMidiDone()) {
			while (midi.getCurrentEvent(e) && e.absolute <= currentTick) {
				//cout << "absolute: " << e.absolute << ", channel: " << e.channel << ", note: " << e.note << ", velocity: " << e.velocity << endl;
				noteEvent(e.note,e.velocity,e.bNoteOn);
				midi.nextEvent();
				
				
			}
		} else {
			if (demo.getState()) {
				demo.setState(0);
			}
		}
		
		*/
		ofSleepMillis(10);
		
	}
}



//--------------------------------------------------------------
void testApp::update()
{
	
	
			
}

//--------------------------------------------------------------
void testApp::draw()
{
	
}

void testApp::exit() {
	thread.ProcOff();
	stopThread();
	sleep(100);
	
	printf("exit()\n");
	for (int i=0; i<kNumberPlayers; i++) {
		players[i]->exit();
	}
}

//--------------------------------------------------------------
void testApp::touchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i down at (%i,%i)\n", touchId, x,y);
	
}
//--------------------------------------------------------------
void testApp::touchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i moved at (%i,%i)\n", touchId, x,y);
	
}
//--------------------------------------------------------------
void testApp::touchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i up at (%i,%i)\n", touchId, x,y);
}
//--------------------------------------------------------------
void testApp::touchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data){
	printf("touch %i double tap at (%i,%i)\n", touchId, x,y);
}
//--------------------------------------------------------------
void testApp::lostFocus()
{
	cout<<"lostFocus"<<endl;
}
//--------------------------------------------------------------
void testApp::gotFocus()
{
	cout<<"gotFocus"<<endl;
}
//--------------------------------------------------------------
void testApp::gotMemoryWarning()
{
	cout<<"MemoryWarning"<<endl;
}

