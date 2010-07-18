#include "testApp.h"
#include "ofxRKUtilities.h"
//#include "ofxDirList.h"
//--------------------------------------------------------------

#define LIPS_MODES_NUMBER 6

void testApp::setup(){
	lipsModesNum = LIPS_MODES_NUMBER;
	ofBackground(255,255,255);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	//ofxDirList DIR;
//	DIR.setVerbose(false);
//	DIR.allowExt("mov");
//    numberOfMovies = DIR.listDir("movies/.");
	
	paths = ofListFolders(ofToDataPath("sequences/."));
	numberOfMovies = paths.size();
 	
	if (!numberOfMovies) {
		std::exit(0);
	}
	
	lips.loadTracks("lips.xml");
	
	rect.x = 0;
	rect.y = 0;
	rect.width = 56;
	rect.height = 42;
	
	//printf("%u",xml.tagExists("textures"));
	currentMovie = 0;
	//filename = DIR.getName(currentMovie);
//	movie.loadMovie(DIR.getPath(currentMovie));
	//movie.setLoopState(OF_LOOP_NONE);
	filename = paths.at(currentMovie);
	lips.setCurrentTrack(filename);
	movie.loadMovie("sequences/"+filename);
	baseFrameMovie.loadMovie("sequences/"+filename);
	float mw = movie.getWidth();
	float mh = movie.getHeight();
	atlas.setup(filename,mw,mh,movie.getTotalNumFrames()/lipsModesNum);
	lipsAtlas.setup(filename+"_LIPS",rect.width,rect.height,movie.getTotalNumFrames());
	
	movie.play();
	baseFrameMovie.play();
	baseFrameMovie.setPaused(true);
	
	frameByFrame = false;
	
	
	
	
	moviePos.x = 20;
	moviePos.y = 160;
	bDragRect = false;
	

}

void testApp::exit() {
	
}

void testApp::updateRect() {
	int cur = movie.getCurrentFrame();
	int x;
	int y;
	if (lips.getPoint((cur - cur % lipsModesNum) / lipsModesNum , x, y)) {
		rect.x = x;
		rect.y = y;
	}
}

//--------------------------------------------------------------
void testApp::update(){
	
    movie.idleMovie();
	baseFrameMovie.idleMovie();
	baseFrameMovie.setFrame(movie.getCurrentFrame() / LIPS_MODES_NUMBER * LIPS_MODES_NUMBER);
	//grab.update();
	
	/*
	if (!bGrab && !frameByframe && movie.getCurrentFrame()>endFrame) {
		movie.setPaused(true);
		movie.setFrame(startFrame);
		movie.setPaused(false);
	}
	*/ 
	
	if (!frameByFrame && movie.isFrameNew()) {
		updateRect();
		
	}
	
	/*
	if (bStore && movie.isFrameNew()) {
		ofImage *image = new ofImage;
		images.push_back(image);
		image->setFromPixels(movie.getPixels(),movie.getWidth(),movie.getHeight(),true);
		
		if (movie.getCurrentFrame() == movie.getTotalNumFrames() - 1 ) {
			bStore = false;
		}
		
	}
	 */
	
	/*
	if(bGrab && movie.getCurrentFrame()>endFrame){
		bGrab = false;
		if ((grabFrameNum) % (rows*columns) != 0) {
			addImage();
		}
    }
	*/
}



		
//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(0x000000);
	char str[250];
	
	sprintf(str, "sequences: %s (%u/%u)",filename.c_str(),currentMovie+1,numberOfMovies);
	ofDrawBitmapString(string(str), 20, 20);
	sprintf(str,"frame: %u/%u, pos: %.2f/%.2f",movie.getCurrentFrame(),movie.getTotalNumFrames(),movie.getPosition(),movie.getDuration());
	ofDrawBitmapString(string(str), 20, 40);
	sprintf(str, "rect - width: %.0f, height: %.0f",rect.width,rect.height);
	ofDrawBitmapString(string(str), 20, 60);

	//sprintf(str, "width: %u, height: %u, stored: %u",movie.getWidth(),movie.getHeight(),(int)images.size());
	sprintf(str, "width: %u, height: %u",movie.getWidth(),movie.getHeight());
	ofDrawBitmapString(string(str), 20, 80);
	
	
	/*
	int col = 1024/movie.getWidth();
	int row = 1024/movie.getHeight();
	int framesPerTexture = col*row;
	int numTextures = movie.getTotalNumFrames() / framesPerTexture;
	int rem = movie.getTotalNumFrames() % framesPerTexture;
	sprintf(str, "big textures: %u (%u frames)",numTextures,framesPerTexture);
	ofDrawBitmapString(str, 20, 80);
	
	sprintf(str, "remainder: %u",rem);
	ofDrawBitmapString(str, 20, 100);
	 */
	
	ofDrawBitmapString(atlas.getDebugStr(), 20, 100);
	ofDrawBitmapString(lipsAtlas.getDebugStr(), 420, 100);
	
	
	ofSetColor(0xFFFFFF);
	
	movie.draw(moviePos.x,moviePos.y);
	baseFrameMovie.draw(moviePos.x+movie.getWidth()+20,moviePos.y);
	rectImage.grabScreen(moviePos.x+rect.x,moviePos.y+rect.y, rect.width, rect.height);
	rectImage.draw(moviePos.x+movie.getWidth()+20+rect.x,moviePos.y+rect.y);
	//movie.draw(moviePos.x+movie.getWidth()+20+rect.x,moviePos.y+rect.y);//,rect.width,rect.height);
	ofEnableAlphaBlending();
	ofSetColor(255,0,0,127);   // red, 50% transparent
	ofRect(moviePos.x+rect.x,moviePos.y+rect.y,rect.width,rect.height);
	ofDisableAlphaBlending();
	
	
	//grab.draw(0, 0);
      
	    
	/*
    if(movie.getIsMovieDone()){
        ofSetColor(0xFF0000);
        ofDrawBitmapString("end of movie",20,440);
    }
	
	ofDrawBitmapString("images: " + ofToString((int)images.size()),20,460);
    ofDrawBitmapString("frame number: " + ofToString(grabFrameNum),20,480);
	ofDrawBitmapString("x: "+ofToString((int)(frameRect.x))+", y: "+ofToString((int)(frameRect.y))+"width: "+ofToString((int)(1024/columns))+", height: "+ofToString((int)(1024/rows)), 20, 500);
	ofDrawBitmapString("fps: " + ofToString((float)movie.getTotalNumFrames()/(float)movie.getDuration(),2),20,520);
	
	
	ofSetColor(0xff0000);
	ofNoFill();
	ofRect(frameRect.x,frameRect.y,frameRect.width,frameRect.height);
	*/
	
	
}

	

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	
	int lastMovie = currentMovie;
	
	switch (key) {
		/*
		case 's':
			
				bStore = true;
				movie.setPaused(true);
				movie.firstFrame();
				movie.setPaused(false);
			movie.idleMovie();
			
			break;
		 */
		case 'g':
			
			atlas.process("images/"+filename,movie);
			lipsAtlas.process("images/"+filename+"_LIPS", movie, &(lips.getCurrentTrack().points));
			atlas.save();
			lipsAtlas.save();
			
			break;
			
		case 'j':
			
			atlas.process("images/"+filename,movie);
			atlas.save();
			
			break;	
		
		case 'f':
			frameByFrame = !frameByFrame;
			movie.setPaused(frameByFrame);
			break;
			
		case '-':
			rect.width-=1;
			break;
		
		case '=':
			rect.width+=1;
			break;
			
		case '[':
			rect.height-=1;
			break;
			
		case ']':
			rect.height+=1;
			break;

	}
	
	switch (key) {
		case '-':
		case '=':
		case '[':
		case ']':
			//lipsAtlas.setup(filename+"_LIPS",rect.width,rect.height,movie.getTotalNumFrames());
			lipsAtlas.update(rect.width,rect.height);
			lipsAtlas.save();
			break;
		default:
			break;
	}
	
	
	if (!frameByFrame) {
		switch (key) {
			case OF_KEY_LEFT:
				currentMovie = (currentMovie-1+numberOfMovies) % numberOfMovies;
				break;
			case OF_KEY_RIGHT:
				currentMovie = (currentMovie+1) % numberOfMovies;
				break;
			case OF_KEY_UP:
				
				break;
			case OF_KEY_DOWN:
				
				break;
				
			default:
				break;
				
		}
	} else {
		
			
		switch (key) {
			case OF_KEY_LEFT:
				movie.previousFrame();
				updateRect();
				//movie.setFrame((movie.getCurrentFrame() - lipsModesNum + movie.getTotalNumFrames()) % movie.getTotalNumFrames());
				break;
			case OF_KEY_RIGHT:
				movie.nextFrame();
				updateRect();
				//movie.setFrame(baseFrame+(movie.getCurrentFrame() + lipsModesNum) % movie.getTotalNumFrames());
				break;
			case OF_KEY_UP:
				break;
			case OF_KEY_DOWN:
				
				break;
				
			default:
				break;
				
		}
	}

	if (currentMovie != lastMovie || key=='s') {
		int numFrames = movie.getTotalNumFrames() / lipsModesNum;
		if (numFrames != lips.getNumPoints()) {
			int x;
			int y;
			if (lips.getPoint(lips.getNumPoints()-1, x, y)) {
				lips.updatePoint(numFrames-1, x, y);
			}
			
		}
		lips.saveTracks("lips.xml");
		//filename = DIR.getName(currentMovie);
//		movie.loadMovie(DIR.getPath(currentMovie));
		filename = paths.at(currentMovie);
		lips.setCurrentTrack(filename);
		movie.loadMovie("sequences/"+filename);
		baseFrameMovie.loadMovie("sequences/"+filename);
		//movie.setLoopState(OF_LOOP_NONE);
		float mw = movie.getWidth();
		float mh = movie.getHeight();
		atlas.setup(filename,mw,mh,movie.getTotalNumFrames()/lipsModesNum);
		lipsAtlas.setup(filename+"_LIPS",rect.width,rect.height,movie.getTotalNumFrames());
		movie.play();
		
		/*
		for (vector<ofImage*>::iterator frame=images.begin(); frame!=images.end(); frame++) {
			(*frame)->clear();
			delete (*frame);
		}
		images.clear();
		*/
	}
	
	
		
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	/*
	if(!frameByframe){
        int width = ofGetWidth();
        float pct = (float)x / (float)width;
        float speed = (2 * pct - 1) * 5.0f;
        movie.setSpeed(speed);
	}
	 */
	
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	x-=moviePos.x;
	y-=moviePos.y;
	if (bDragRect) {
		rect.x = x - mouseDiff.x;
		rect.y = y - mouseDiff.y;
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	x-=moviePos.x;
	y-=moviePos.y;
	mouseDiff.x = x - rect.x ;
	mouseDiff.y = y - rect.y ;
	if ( mouseDiff.x >=0 && mouseDiff.x <=rect.width  && mouseDiff.y >=0 && mouseDiff.y <=rect.height) {
		bDragRect = true;
		
	}
	
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
	if (bDragRect) {
		bDragRect = false;
		int cur = movie.getCurrentFrame();
		lips.updatePoint((cur - cur % lipsModesNum) / lipsModesNum , rect.x, rect.y);
	}
	
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){
	
}


