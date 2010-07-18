#include "testApp.h"
#include "ofxDirList.h"
#include "ofxXmlSettings.h"
#include "ofxMacUtils.h"

// listen on port 12345
#define PORT 12345
#define NUM_GROUPS 4

void control::setup() {
	state = 0;
	bChanged = false;
}

int control::getState() {
	return state;
}

void control::setState(int state) {
	this->state = state;
}

void control::draw() {
	images[state]->draw(x,y);
}

bool control::didChange() {
	return bChanged;
}

void control::resetChanged() {
	bChanged = false;
}
	


void toggleButton::setup(string name,int statesNum,int x,int y) {
	control::setup();
	
	for (int i=0;i<statesNum;i++) {
		ofImage *image = new ofImage;
		image->loadImage(name+ofToString(i)+".png");
		images.push_back(image);
	}
	
	this->x = x;
	this->y = y;
	this->statesNum = statesNum;
	
}


void toggleButton::mousePressed(int x, int y, int button) {
	x-=this->x;
	y-=this->y;
	if (x>=0 && x<=(*images.begin())->getWidth() && y>=0 && y<=(*images.begin())->getHeight()) {
		int last = state;
		state = x / ((*images.begin())->getWidth() / statesNum);
		if (last!=state) {
			bChanged = true;
		}
	}
}




void switchButton::setup(string name,int x,int y) {
	control::setup();
	for (int i=0;i<2;i++) {
		ofImage *image = new ofImage;
		image->loadImage(name+ofToString(i)+".png");
		images.push_back(image);
	}
	
	this->x = x;
	this->y = y;
}


void switchButton::mousePressed(int x, int y, int button) {
	x-=this->x;
	y-=this->y;
	if (x>=0 && x<=(*images.begin())->getWidth() && y>=0 && y<=(*images.begin())->getHeight()) {
		
		state = (state + 1) % 2;
		
		bChanged = true;
		
	}
	
}




//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(255,255,255);
	
	
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup( PORT );

	background.loadImage("BACKGROUND.png");
	state.setup("toggle", 3, 280, 640);
	demo.setup("switch", 45, 640);
	state.setState(1);
	thread.AddObj(&mix);
	
	loadFromXml();
	refresh();
	
	noteIter = notes.end();
	bScaling = false;
	
	
	scale = 1.0;
		
	output.SetOutput(2, &mix);
	
	
	thread.AddObj(&output,SNDIO_OUT);
	
	
	midi.loadFromXml("Muppet_Show.xml");
	
	/*
	event e;
	while (midi.getCurrentEvent(e)) {
		cout << "absolute: " << e.absolute << ", channel: " << e.channel << ", note: " << e.note << ", velocity: " << e.velocity << endl;
		midi.nextEvent();
	}
	 */
	
	
	thread.ProcOn();
	
	bEditMode = false;
	
}

/*
void testApp::threadedFunction(){
	
	while( isThreadRunning() != 0 ){
		if( lock() ){
			//int currentTick = (ofGetElapsedTimeMillis() - startTime)  *  midi.getTicksPerBeat() * 120 / 1000 / 60;
			int currentTick = (ofGetElapsedTimeMillis() - startTime)  *  midi.getTicksPerBeat() / 500;
			event e;
			while (midi.getCurrentEvent(e) && e.absolute <= currentTick) {
				//cout << "absolute: " << e.absolute << ", channel: " << e.channel << ", note: " << e.note << ", velocity: " << e.velocity << endl;
				noteEvent(e.note,e.velocity);
				midi.nextEvent();
				
				
			}
			
			unlock();
			ofSleepMillis(1 * 1000);
		}
	}
}
*/

void testApp::threadedFunction(){
	
	while( isThreadRunning() != 0 ){
		
		//int currentTick = (ofGetElapsedTimeMillis() - startTime)  *  midi.getTicksPerBeat() * 120 / 1000 / 60;
		int currentTick = (ofGetElapsedTimeMillis() - startTime)  *  midi.getTicksPerBeat() / 500;
		event e;
		
		int done = 0;
		
		for (int j=0;j<midi.getNumTracks();j++) {
			if (!midi.getIsMidiDone(j)) {
				while (midi.getCurrentEvent(j,e) && e.absolute <= currentTick) {
					//cout << "absolute: " << e.absolute << ", channel: " << e.channel << ", note: " << e.note << ", velocity: " << e.velocity << endl;
					noteEvent(e.note,e.velocity,e.bNoteOn);
					midi.nextEvent(j);
					
					
				}
			} else 
				done++;
		}
			
		if (done==midi.getNumTracks()) {
			if (demo.getState()) {
				demo.setState(0);
			}
		}

		
		ofSleepMillis(10);
		
	}
}


void testApp::loadFromXml() {
	
	ofxXmlSettings xml;
	
	if (!xml.loadFile("layout.xml"))
		return;
	
	xml.pushTag("layout");
	
	for (int i=0; i<xml.getNumTags("note"); i++) {
		note *aNote = new note; 
		aNote->midi = xml.getAttribute("note", "midi", 0, i);
		aNote->x = xml.getAttribute("note", "x", 0, i);
		aNote->y = xml.getAttribute("note", "y", 0, i);
		aNote->scale = xml.getAttribute("note", "scale", 1.0, i);
		aNote->start = xml.getAttribute("note", "start", 0, i);
		aNote->group = xml.getAttribute("note", "group", 0, i);
		aNote->key = xml.getAttribute("note", "key", '1', i);
		
		//aNote->bIdle = aNote->idle.loadMovie("IDLE_"+ofToString(aNote->midi)+".mov");
		//aNote->bNoteOn = aNote->noteOn.loadMovie("NOTE_"+ofToString(aNote->midi)+".mov");
		
		notes[aNote->midi]=aNote;
		if (aNote->midi>36) {
			keys[aNote->key] = aNote;
		} else {
			beatBoxKeys[aNote->key] = aNote;
		}

		
		aNote->bAudio = false;
		aNote->bAudioPlaying = false;
		aNote->bPlaying = false;
	}
}


void testApp::saveToXml() {
	ofxXmlSettings xml;

	xml.addTag("layout");	
	xml.pushTag("layout");
	
	
	
	for (map<int,note*>::iterator iter = notes.begin(); iter!=notes.end(); iter++) {
		int i = xml.addTag("note");
		xml.addAttribute("note", "midi", iter->second->midi, i);
		xml.addAttribute("note", "x", iter->second->x, i);
		xml.addAttribute("note", "y", iter->second->y, i);
		xml.addAttribute("note", "scale", iter->second->scale, i);
		xml.addAttribute("note", "start", iter->second->start, i);
		xml.addAttribute("note", "group", iter->second->group, i);
		xml.addAttribute("note", "key", iter->second->key, i);
		
	}
	
	xml.saveFile("layout.xml");
	
}

void testApp::refresh() {
	ofxDirList DIR;
	DIR.setVerbose(false);
	DIR.allowExt("mov");
    int nMovies = DIR.listDir("./video");
 	
    for(int i = 0; i < nMovies; i++) {
		vector<string> sp1 = ofSplitString(DIR.getName(i),"_");
		if (sp1[0]=="NOTE") {
			vector<string> sp2 = ofSplitString(sp1[1],".");
			int midi = ofToInt(sp2[0]);
			
			map<int,note*>::iterator iter = notes.find(midi);
			
			note *aNote;
			
			if (iter==notes.end()) {
				aNote = new note; 
				aNote->midi = midi;
				aNote->x = 0;
				aNote->y = 0;
				aNote->scale = 1.0;
				aNote->start = 0;
				
				aNote->bPlaying = false;
				notes[aNote->midi]=aNote;
				
			} else {
				aNote = iter->second;
			}

			
			
			
			aNote->bNoteOn = aNote->noteOn.loadMovie("video/NOTE_"+ofToString(aNote->midi)+".mov");
			aNote->noteOn.play();
			aNote->noteOn.setLoopState(OF_LOOP_NONE);
			aNote->noteOn.setPaused(true);
			aNote->noteOn.setFrame(aNote->start);
			
			
			aNote->bPlaying = false;
			aNote->bStartVideo = false;
			aNote->bStopVideo = false;
			

			//cout << aNote->midi;
			

		}
	}
	
	DIR.reset();
	DIR.allowExt("png");
    int nImages = DIR.listDir("./images");
 	
    for(int i = 0; i < nImages; i++) {
		vector<string> sp1 = ofSplitString(DIR.getName(i),"_");
		
		if (sp1[0]=="IDLE") {
			vector<string> sp2 = ofSplitString(sp1[1],".");
			int midi = ofToInt(sp2[0]);
			
			map<int,note*>::iterator iter = notes.find(midi);
			
			note *aNote;
			
			if (iter==notes.end()) {
				aNote = new note; 
				aNote->midi = midi;
				aNote->x = 0;
				aNote->y = 0;
				aNote->scale = 1.0;
				aNote->start = 0;
				
				aNote->bPlaying = false;
				notes[aNote->midi]=aNote;
				
			} else {
				aNote = iter->second;
			}
			
			
			aNote->bIdle = aNote->idle.loadImage("images/IDLE_"+ofToString(aNote->midi)+".png");
						
			aNote->bPlaying = false;
			aNote->bStartVideo = false;
			aNote->bStopVideo = false;
			
			
			//cout << aNote->midi;
			
		}
	}
	
	DIR.reset();
	DIR.allowExt("aif");
	int nAudios = DIR.listDir("./audio");
	
	for(int i = 0; i < nAudios; i++) {
		vector<string> sp1 = ofSplitString(DIR.getName(i),"_");
		if (sp1[0]=="NOTE") {
			vector<string> sp2 = ofSplitString(sp1[1],".");
			int midi = ofToInt(sp2[0]);
			
			map<int,note*>::iterator iter = notes.find(midi);
			
			note *aNote;
			
			if (iter==notes.end()) {
				aNote = new note; 
				aNote->midi = midi;
				aNote->x = 0;
				aNote->y = 0;
				aNote->scale = 1.0;
				aNote->start = 0;
				
				aNote->bPlaying = false;
				notes[aNote->midi]=aNote;
				
			} else {
				aNote = iter->second;
			}
			
			/*
			if (aNote->bAudio) {
				aNote->input.SetInput(0);
				delete aNote->audio;
				aNote->audio = 0;
			}
			 */
			
			string str = ofToDataPath("audio/NOTE_"+ofToString(aNote->midi)+".aif");
			char * name = const_cast<char*>(str.c_str());
			aNote->audio = new SndAiff(name,READ);				
			aNote->input.SetInput(aNote->audio, 1);
			aNote->input.Disable();
			aNote->gain.SetInput(&aNote->input);
			aNote->gain.SetGain(-10);
			mix.AddObj(&aNote->gain);
			
			aNote->bAudio = true;
			aNote->bAudioPlaying = false;
			
			thread.AddObj(aNote->audio,SNDIO_IN);
			thread.AddObj(&aNote->input);
			thread.Insert(&aNote->gain,&aNote->input);
						
			//aNote->audio.loadSound("NOTE_"+ofToString(aNote->midi)+".aif",true);
			//cout << aNote->midi;
			
			
		}
		
	
	}
		
		
		//images[i].loadImage(DIR.getPath(i));
    
    
	
}

void testApp::exit() {
	thread.ProcOff();
	stopThread();
	
	sleep(1);
	
	for(map<int,note*>::iterator iter=notes.begin();iter!=notes.end();iter++) 
		if (iter->second->bAudio) 
			delete iter->second->audio;
	
}

//--------------------------------------------------------------
void testApp::update() {
	
	
	
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		if ( m.getAddress() == "/noteout" && state.getState() == 0)
		{
			int midiNote = m.getArgAsInt32( 0 );
			int velocity = m.getArgAsInt32( 1 );
			
			noteEvent(midiNote, velocity,velocity > 0);
		
		}
	}
	
	for(map<int,note*>::iterator iter=notes.begin();iter!=notes.end();iter++) {
		
		if (iter->second->bStartVideo) {
			iter->second->bStartVideo = false;
			//iter->second->idle.setPaused(true);
						iter->second->noteOn.setPaused(false);
			iter->second->bPlaying = true;
		}
		
		if (iter->second->bStopVideo) {
			iter->second->bStopVideo = false;
			//iter->second->idle.firstFrame();
			//iter->second->idle.setPaused(false);
			iter->second->noteOn.setPaused(true);
			iter->second->noteOn.setFrame(iter->second->start);

			iter->second->bPlaying = false;
			
		}
		
		if (iter->second->bIdle) 
			iter->second->idle.update();
		if (iter->second->bNoteOn)
			iter->second->noteOn.update();
		
		
	}
	
	
	
	
	 
}


void testApp::noteEvent(int midiNote,int velocity,bool bNoteOn) {
	
	cout << "midiNote: " << midiNote << ", velocity: " << velocity << endl;
	
	map<int,note*>::iterator iter = notes.find(midiNote);
	
	if (iter!=notes.end()) {
		if (bNoteOn) {
			iter->second->bStartVideo = true;
			iter->second->bAudioPlaying = true;
			
			if (iter->second->audio) {
				iter->second->audio->SetPos(0.0f);
				iter->second->input.Enable();
				//iter->second->gain.SetGain(-10);					
			}
			
		} else {
			iter->second->bStopVideo = true;
			
			iter->second->bAudioPlaying = false;
			
			
			iter->second->input.Disable();
			//iter->second->gain.SetGain(-90);
		}
	}
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofScale(scale, scale, 0);
	
	ofSetColor(0xffffff);
	
	
	background.draw(0, 0);
	
	
	ofEnableAlphaBlending();
	
	state.draw();
	demo.draw();
	
	map<int,note*>::iterator iter;
	
	for (int i=0; i<NUM_GROUPS; i++) 
		for(iter=notes.begin();iter!=notes.end();iter++)
			if (iter->second->bIdle && iter->second->group==i) {
				
				ofPushMatrix();
				ofTranslate(iter->second->x,iter->second->y, 0);
				if (!iter->second->bPlaying) {
					ofScale(iter->second->scale, iter->second->scale, 1.0);
					iter->second->idle.draw(0,0);
				} else {
					ofScale(iter->second->scale, iter->second->scale, 1.0);
					iter->second->noteOn.draw(0,0);
				}
				ofPopMatrix();
			}
		
		
	/*
	
	for (int i=0; i<NUM_GROUPS; i++) 
		for(iter=notes.begin();iter!=notes.end();iter++)
			if (iter->second->bNoteOn && iter->second->group==i && iter->second->bPlaying ) {
				ofPushMatrix();
				ofTranslate(iter->second->x,iter->second->y, 0);
				ofScale(iter->second->scale*1.10, iter->second->scale*1.25, 1.25);
				iter->second->noteOn.draw(0,0);
				ofPopMatrix();
			}
		
	*/
	
	
	ofDisableAlphaBlending();
	
	for(map<int,note*>::iterator iter=notes.begin();iter!=notes.end();iter++) {
		if (iter->second->bPlaying && iter->second->noteOn.getIsMovieDone()) {
			//iter->second->idle.firstFrame();
			//iter->second->idle.setPaused(false);
			iter->second->noteOn.setPaused(true);
			iter->second->noteOn.firstFrame();
			iter->second->bPlaying = false;
		}
	}
	
	ofSetColor(0x00ff00);
	
	if (bEditMode) {
	
		if (noteIter!=notes.end() ) {
			
			ofDrawBitmapString(ofToString(noteIter->second->midi),120,20);
			ofDrawBitmapString(ofToString(noteIter->second->start),120,40);
			ofDrawBitmapString(ofToString(noteIter->second->group),120,60);
		}
		
		
		ofDrawBitmapString(ofToString(scale,3),20,20);
	}
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (state.getState()) {
		case 0:
			if (key=='e') {
				bEditMode = !bEditMode;
			} if (key=='o') {
				ofDisableDataPath();
				bool res = midi.loadFromXml(ofxFileDialog());
				ofEnableDataPath();
				if (!res)
					midi.loadFromXml("Muppet_Show.xml");
			}else {
				if (bEditMode) {
					switch (key) {
						
						case 's':
							saveToXml();
							break;
						case 'c':
							bScaling = !bScaling;
							//if (noteIter!=notes.end()) {
							//	noteIter->second->scale += 0.2;
							//}
							break;
							
						case '-':
							if (noteIter!=notes.end()) {
								noteIter->second->start--;
							}
							break;
						case '=':
							if (noteIter!=notes.end()) {
								noteIter->second->start++;
							}
							break;
						case '[':
							if (noteIter!=notes.end() && noteIter->second->group > 0) {
								noteIter->second->group--;
							}
							break;
							
						case ']':
							if (noteIter!=notes.end() && noteIter->second->group < NUM_GROUPS-1) {
								noteIter->second->group++;
							}
							break;
							
						case 't': 
							ofToggleFullscreen();
							break;
						case ',':
							scale-=0.1;
							break;
							
						case '.':
							scale+=0.1;
							break;
							
							
						default:
							break;
					}
				}
			}

			
			
			break;
		case 1: {
			map<int,note*>::iterator iter = keys.find(key);
			if (iter!=keys.end()) {
				if (!iter->second->bAudioPlaying) {
					noteEvent(iter->second->midi, 128,true);
				}
			}
		} break;
		case 2: {
			map<int,note*>::iterator iter = beatBoxKeys.find(key);
			if (iter!=beatBoxKeys.end()) {
				if (!iter->second->bAudioPlaying) {
					noteEvent(iter->second->midi, 128,true);
				}
			}
		} break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	switch (state.getState()) {
		case 1: {
			map<int,note*>::iterator iter = keys.find(key);
			if (iter!=keys.end()) {
				noteEvent(iter->second->midi, 0,false);		
			}
		} break;
		
		case 2: {
			map<int,note*>::iterator iter = beatBoxKeys.find(key);
			if (iter!=beatBoxKeys.end()) {
				noteEvent(iter->second->midi, 0,false);		
			}
		} break;
	}
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
	if (bEditMode) {
		int dx = (x-downX);
		int dy = (y-downY);
		
		if (noteIter!=notes.end()) {
			if (bScaling) {
				
				int dx0 = (downX-x0) / s0; 
				int dy0 = (downY-y0) / s0; 	
				
				float width = noteIter->second->idle.getWidth();
				float downWidth = width * s0;
				float s1 = s0 * float(downWidth+dx*s0)/downWidth; 
				noteIter->second->scale = s1;
				
				//int dx1 = (downX-x1) / s1; 
				//int dy1 = (downY-y1) / s1;
				
				noteIter->second->x = downX - dx0 *s1;
				noteIter->second->y = downY - dy0 *s1;
				
				
				
			} else {
				
				
				noteIter->second->x = x0 + dx;
				noteIter->second->y = y0 + dy;
			}
			
		}
		
	}
		
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	state.mousePressed(x, y, button);
	demo.mousePressed(x, y, button);
	
	if (demo.didChange()) {
		demo.resetChanged();
		if (demo.getState()) {
			for (int j=0;j<midi.getNumTracks();j++)
				midi.firstEvent(j);
			startTime = ofGetElapsedTimeMillis();
			startThread(false, false);   // non blocking, verbose
		} else {
			stopThread();
		}
	}
	
	if (bEditMode) {
		downX = x;
		downY = y;
		
		for(noteIter=notes.begin();noteIter!=notes.end();noteIter++) {
			if (noteIter->second->bIdle) {
				int dx = (x-noteIter->second->x) / noteIter->second->scale; 
				int dy = (y-noteIter->second->y) / noteIter->second->scale; 
				
				if (dx > 0 && dx<= noteIter->second->idle.getWidth() && dy > 0 && dy <= noteIter->second->idle.getHeight() ) {
					unsigned char * pixels = noteIter->second->idle.getPixels();
					
					unsigned char alpha = pixels[(int)noteIter->second->idle.getWidth() * 4*dy + 4 * dx + 3];
					
					s0 = noteIter->second->scale;
					x0 = noteIter->second->x;
					y0 = noteIter->second->y;
					char str[255];
					sprintf(str, "scale: %4.2f, dx: %i, dy: %i, alpha: %i\n",s0,dx,dy,alpha);
					cout << str;
					if (alpha > 0) {
						break;
					}
				}
			}
			
		}
	}
	
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	noteIter = notes.end();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

