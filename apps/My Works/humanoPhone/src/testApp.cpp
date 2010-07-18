#include "testApp.h"
#include "ofxDirList.h"
#include "ofxXmlSettings.h"

// listen on port 12345
#define PORT 12345
#define NUM_GROUPS 4

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(255,255,255);
	
	
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup( PORT );

	//background.loadImage("Layout2.jpg");
	//background.loadImage("Layout1.jpg");
	background.loadImage("keyboard.png");
	
	loadFromXml();
	refresh();
	
	noteIter = notes.end();
	bScaling = false;
	bBackground = false;
	
	scale = 1.0;
	
	bPlayingKeyboard = false;
	
	//output.SetOutput(2, &mix);
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
		keys[aNote->key] = aNote;
		//aNote->audio = 0;
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
    int nMovies = DIR.listDir(".");
 	
    for(int i = 0; i < nMovies; i++) {
		vector<string> sp1 = ofSplitString(DIR.getName(i),"_");
		bool bIdle = sp1[0]=="IDLE";
		bool bNoteOn = sp1[0]=="NOTE";
		if (bIdle || bNoteOn) {
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

			if (bIdle && !aNote->bIdle) {
				aNote->bIdle = aNote->idle.loadMovie("IDLE_"+ofToString(aNote->midi)+".mov");
				
				aNote->idle.play();
				//aNote->noteOn.setLoopState(OF_LOOP_PALINDROME);
			}
			
			if (bNoteOn	&& !aNote->bNoteOn) {
				aNote->bNoteOn = aNote->noteOn.loadMovie("NOTE_"+ofToString(aNote->midi)+".mov");
				aNote->noteOn.play();
				aNote->noteOn.setLoopState(OF_LOOP_NONE);
				aNote->noteOn.setPaused(true);
			}
			
			aNote->bPlaying = false;
			

			//cout << aNote->midi;
			

		}
	}
		
	DIR.reset();
	DIR.allowExt("aif");
	int nAudios = DIR.listDir(".");
	
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
			if (aNote->audio) {
				mix.DeleteObj(&aNote->input);
				delete aNote->audio;
				aNote->audio = 0;
			}
			
			string str = ofToDataPath("NOTE_"+ofToString(aNote->midi)+".aif");
			char * name = const_cast<char*>(str.c_str());
			aNote->audio = new SndAiff(name,READ);				
			aNote->input.SetInput(aNote->audio, 1);
			mix.AddObj(&aNote->input);
			aNote->bAudioPlaying = false;
			*/
			aNote->audio.loadSound("NOTE_"+ofToString(aNote->midi)+".aif",true);
			//cout << aNote->midi;
			
			
		}
		
	
	}
		
		
		//images[i].loadImage(DIR.getPath(i));
    
    
	
}

//--------------------------------------------------------------
void testApp::update() {
	
	
	
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		if ( m.getAddress() == "/noteout" )
		{
			int midi = m.getArgAsInt32( 0 );
			int velocity = m.getArgAsInt32( 1 );
			
			cout << "midi: " << midi << ", velocity: " << velocity << endl;
			
			map<int,note*>::iterator iter = notes.find(midi);
			
			if (iter!=notes.end()) {
				if (velocity) {
					iter->second->idle.setPaused(true);
					iter->second->noteOn.setFrame(iter->second->start);
					iter->second->noteOn.setPaused(false);
					iter->second->bPlaying = true;
					iter->second->bAudioPlaying = true;
					
					//if (iter->second->audio)
//						iter->second->audio->SetPos(0.0f);
					iter->second->audio.play();
				} else {
					iter->second->idle.firstFrame();
					iter->second->idle.setPaused(false);
					iter->second->noteOn.setPaused(true);
					iter->second->bPlaying = false;
					iter->second->bAudioPlaying = false;
					
					iter->second->audio.stop();
				}
			}
			
			

		}
	}
	
	for(map<int,note*>::iterator iter=notes.begin();iter!=notes.end();iter++) {
		if (iter->second->bIdle) 
			iter->second->idle.update();
		if (iter->second->bNoteOn)
			iter->second->noteOn.update();
	}
	
	/*
	for (int i=0; i<20; i++) {
		for(map<int,note*>::iterator iter=notes.begin();iter!=notes.end();iter++) {
			if (iter->second->audio) {			
				if (iter->second->bAudioPlaying) {
					if (iter->second->audio->Eof()) {
						iter->second->bAudioPlaying = false;
					}
					else {
						iter->second->audio->Read();
					}
				}
			
			
			
				iter->second->input.DoProcess();
			}
			
		}
		mix.DoProcess();
		output.Write();
	}
	*/
	
	
	 
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofScale(scale, scale, 0);
	
	ofSetColor(0xffffff);
	
	if (bBackground) 
		background.draw(0, 0);
	
	ofEnableAlphaBlending();
	
	map<int,note*>::iterator iter;
	
	for (int i=0; i<NUM_GROUPS; i++) 
		for(iter=notes.begin();iter!=notes.end();iter++)
			if (iter->second->bIdle && iter->second->group==i && !iter->second->bPlaying) {
				ofPushMatrix();
				ofTranslate(iter->second->x,iter->second->y, 0);
				ofScale(iter->second->scale, iter->second->scale, 1.0);
				iter->second->idle.draw(0,0);
				ofPopMatrix();
			}
		
		
	for (int i=0; i<NUM_GROUPS; i++) 
		for(iter=notes.begin();iter!=notes.end();iter++)
			if (iter->second->bNoteOn && iter->second->group==i && iter->second->bPlaying ) {
				ofPushMatrix();
				ofTranslate(iter->second->x,iter->second->y, 0);
				ofScale(iter->second->scale*1.10, iter->second->scale*1.25, 1.25);
				iter->second->noteOn.draw(0,0);
				ofPopMatrix();
			}
		
	
	
	
	ofDisableAlphaBlending();
	
	for(map<int,note*>::iterator iter=notes.begin();iter!=notes.end();iter++) {
		if (iter->second->bPlaying && iter->second->noteOn.getIsMovieDone()) {
			iter->second->idle.firstFrame();
			iter->second->idle.setPaused(false);
			iter->second->noteOn.setPaused(true);
			iter->second->bPlaying = false;
		}
	}
	
	ofSetColor(0x00ff00);
	
	if (noteIter!=notes.end()) {
		
		ofDrawBitmapString(ofToString(noteIter->second->midi),120,20);
		ofDrawBitmapString(ofToString(noteIter->second->start),120,40);
		ofDrawBitmapString(ofToString(noteIter->second->group),120,60);
	}
	
	if (bBackground) 
		ofDrawBitmapString(ofToString(scale,3),20,20);
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (!bPlayingKeyboard) {
		
	
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
			case 'b':
				bBackground = !bBackground;
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
				
			case 'p':
				bPlayingKeyboard = true;
				break;
				
			
			default:
				break;
		}
	} else {
		map<int,note*>::iterator iter = keys.find(key);
		if (iter!=keys.end()) {
			iter->second->idle.setPaused(true);
			iter->second->noteOn.setFrame(iter->second->start);
			iter->second->noteOn.setPaused(false);
			iter->second->bPlaying = true;
			//iter->second->audio.play();
		}
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if (bPlayingKeyboard) {
		map<int,note*>::iterator iter = keys.find(key);
		if (iter!=keys.end()) {
			iter->second->idle.firstFrame();
			iter->second->idle.setPaused(false);
			iter->second->noteOn.setPaused(true);
			iter->second->bPlaying = false;
			//iter->second->audio.stop();
		}
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
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

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
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

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	noteIter = notes.end();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

