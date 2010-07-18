#include "AudioDefs.h" // must define PI before of


#include "testApp.h"
#include "ofxXmlSettings.h"


#include "ofxRKUtils.h"
#include "Player.h"
#include "SndCoreAudio.h"

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
	
	mix = new Mixer();
	Player *player;
	
	xml.pushTag("dead");
	
	xml.pushTag("guitar",0);
	player=new Player();
	player->setup(&xml,0,mix);
	players.push_back(player);
	xml.popTag();
	
	xml.pushTag("vocal",0);
	player=new Player();
	player->setup(&xml,0,mix);
	players.push_back(player);
	xml.popTag();
	
	xml.pushTag("drum",0);
	player=new Player();
	player->setup(&xml,0,mix);
	players.push_back(player);	
	xml.popTag();
	 
	xml.popTag();
	
	
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
	
	
	currentFrame = 0;
	string filename = ofToDocumentsPath(outputfn);
	
	output = new SndCoreAudio(); //1,SND_OUTPUT);
	output->SetOutput(1, mix);
	
	bool bExit;
	
	do 
	{
		int tick = currentFrame * DEF_VECSIZE * ticksPerBeat * (120 / 60) / (int) DEF_SR;
		//cout << "tick: " << tick << endl;
		
		for (int i=0; i<kNumberPlayers; i++) {
			bExit = bExit && players[i]->getIsSongDone();
			if (!bExit)
				break;
		}
		
		if (!bExit) {
			for (int i=0; i<kNumberPlayers; i++) 
				players[i]->update(tick);
			mix->DoProcess();
			output->Write();
			
		}
		currentFrame++;
	} while (!bExit) ;	
	
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

