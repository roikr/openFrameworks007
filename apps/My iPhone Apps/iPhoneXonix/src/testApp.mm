#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);

	ofxiPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	//int 			w, h;
	
	w = ofGetWidth();
	h = ofGetHeight();
	
	imagen = cvCreateImage( cvSize(w,h), IPL_DEPTH_8U, 1 );
	
	texSize = 512;
	tex.allocate(texSize,texSize,GL_RGBA);
	
	fbo.setup();
	
	pixels			= new unsigned char [texSize*texSize*4];
	
	//texGray.loadData(grayPixels, w,h, GL_LUMINANCE); 
	vector<ofPoint> points;
	points.push_back(ofPoint(20,20));
	points.push_back(ofPoint(ofGetWidth()-20,20));
	points.push_back(ofPoint(ofGetWidth()-20,ofGetHeight()-20));
	points.push_back(ofPoint(20,ofGetHeight()-20));
	points.push_back(ofPoint(20,20));
	
	for (vector<ofPoint>::iterator piter=points.begin(); (piter+1)!=points.end(); piter++) {
		ofxLine l = ofxLine(*piter,*(piter+1));
		l.dump();
		lines.push_back(l);
	}
	
	for (int i=0; i<5; i++) {
		evil e;
		e.cpos = e.pos = ofPoint(ofRandom(25, ofGetWidth()-25),ofRandom(25, ofGetHeight()-25));
		e.vel = ofPoint(ofRandom(20, 50)*(ofRandomf()>0.5 ? -1.0f :1.0f), ofRandom(20, 50)*(ofRandomf()>0.5 ? -1.0f :1.0f));
		e.time = ofGetElapsedTimeMillis();
		evils.push_back(e);
	}
	
	bConnected = false;
	bFill = false;
	bFilled = false;
}


//--------------------------------------------------------------
void testApp::update(){
	
	for (vector<evil>::iterator iter=evils.begin(); iter!=evils.end(); iter++) {
		float timeDiff = (ofGetElapsedTimeMillis()-iter->time)/1000.0;
		ofPoint temp = iter->pos+iter->vel*timeDiff;
		
		ofxLine line = ofxLine(iter->cpos,temp);
		vector<ofxLine >::iterator liter;
		for (liter = lines.begin(); liter!=lines.end(); liter++) {
			ofPoint p;
			if (line.getInterception(*liter,p)) {
				float t = (p.x-iter->pos.x)/(temp.x-iter->pos.x);
				cout << t << " " << timeDiff << endl;
				iter->vel *=-1;
				temp = iter->pos = p+iter->vel*(1.0f-t)*timeDiff;
				
				iter->time = ofGetElapsedTimeMillis();
				break;
			}
		}
		
		
		iter->cpos = temp;
		
		
	}
	
	//texGray.loadData(grayPixels, w,h, GL_LUMINANCE); 
	
}

/*
int testApp::getPixelColor(int x,int y) {
	int *current = reinterpret_cast<int*> (pixels+(y*texSize+x)*4) ;
	return *current;
}



void testApp::floodFill(int x,int y,int target,int replacement,int stopCondition) {
	//cout << "." ;
	if (x>ofGetWidth() || x<0 || y>ofGetHeight() || y<0 ) {
		return;
	}
	
	
	
	
	//cout << stopCondition << "(" << pr << " " << pg << " " << pb << ") " << "(" << tr << " " << tg << " " << tb << ") " << "(" << rr << " " << rg << " " << rb << ") " <<endl ;
	
	if (((target >> 16) & 0xff)!=pixels[(y*texSize+x)*4] || ((target >> 8) & 0xff)!=pixels[(y*texSize+x)*4+1] || ((target >> 0) & 0xff)!=pixels[(y*texSize+x)*4+2] ) {
		return;
	}
	
	
	pixels[(y*texSize+x)*4] = ((replacement >> 16) & 0xff);
	pixels[(y*texSize+x)*4+1] = ((replacement >> 8) & 0xff);
	pixels[(y*texSize+x)*4+2] = ((replacement >> 0) & 0xff);
	
	
	
	floodFill(x+1, y,target,replacement,stopCondition+1);
	floodFill(x-1, y,target,replacement,stopCondition+1);
	floodFill(x, y-1,target,replacement,stopCondition+1);
	floodFill(x, y+1,target,replacement,stopCondition+1);
}
 
 */

//--------------------------------------------------------------
void testApp::draw(){
	
	fbo.push(texSize,texSize);
	fbo.begin(tex.texData.textureID);
	
	ofSetColor(0xffffff);
	
	
	
	
	for (vector<ofxLine >::iterator liter = lines.begin(); liter!=lines.end(); liter++) {
		liter->draw();
	}
	
	
/*		
	if (bFill) {
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, texSize, texSize, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		
		floodFill(fill1.x, fill1.y,0x0,0x00ff00);
		
//		for (vector<evil >::iterator eiter = evils.begin(); eiter!=evils.end(); eiter++) {
//			if (getPixelColor(eiter->pos.x, eiter->pos.y)==0x00ff00) {
//				floodFill(fill1.x, fill1.y,0x00ff00,0x0);
//				break;
//			}
//		}
		
//		floodFill(fill2.x, fill2.y,0x0,0x0000ff);
		
//		for (vector<evil >::iterator eiter = evils.begin(); eiter!=evils.end(); eiter++) {
//			if (getPixelColor(eiter->pos.x, eiter->pos.y)==0x0000ff) {
//				floodFill(fill2.x, fill2.y,0x0000ff,0x0);
//				break;
//			}
//		}
		
		tex.loadData(pixels, texSize,texSize, GL_RGBA); 
		bFill = false;
	}
	
*/	
	
		
	if (bFill) {
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		//		glReadPixels(0, 0, texSize, texSize, GL_RGB, GL_UNSIGNED_BYTE, imagen->imageData);
		//		//cvFloodFill(imagen, cvPoint(fill1.x, fill1.y), CV_RGB( 0, 0, 255 ),cvScalarAll(1),cvScalarAll(1),NULL,4  );
		//		
		
		glReadPixels(0, 0, texSize, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		
		for (int i=0; i<h;i++) {
			for (int j=0; j<w; j++) {
				imagen->imageData[i*w+j] = pixels[(i*texSize+j)*4];
			}
		}
		
		vector<evil >::iterator eiter;
		//for (eiter = evils.begin(); eiter!=evils.end(); eiter++) {
//			imagen->imageData[(int)eiter->pos.y * w + (int)eiter->pos.x ]=192;
//		}
		
		IplImage* tempImagen = cvCloneImage(imagen);
		
		cvFloodFill(tempImagen, cvPoint(fill1.x, fill1.y), cvScalarAll(64)  );
		
		for (eiter = evils.begin(); eiter!=evils.end(); eiter++) {
			if ((unsigned char)tempImagen->imageData[(int)eiter->pos.y * w + (int)eiter->pos.x ]==64) {
				break;
			}
		}
		
		if (eiter==evils.end()) {
			imagen = cvCloneImage(tempImagen);
		} else {
			tempImagen = cvCloneImage(imagen);
		}

		
		
		cvFloodFill(tempImagen, cvPoint(fill2.x, fill2.y), cvScalarAll(128)  );
		
		
		for (eiter = evils.begin(); eiter!=evils.end(); eiter++) {
			if ((unsigned char)tempImagen->imageData[(int)eiter->pos.y * w + (int)eiter->pos.x ]==128) {
				break;
			}
		}
		
		if (eiter==evils.end()) {
			imagen = cvCloneImage(tempImagen);
		}
		
		
		
		//cvFloodFill(imagen, cvPoint(fill2.x, fill2.y), cvScalarAll(255)  );
		
		
		
		for (int i=0; i<h;i++) {
			for (int j=0; j<w; j++) {
				pixels[(i*texSize+j)*4] = imagen->imageData[i*w+j];
				
				
			}
		}
		
		//tex.loadData((unsigned char*) (imagen->imageData), texSize, texSize, GL_RGBA );
		tex.loadData(pixels, texSize, h, GL_RGBA);
		bFill = false;
		
	}	
		
		
	fbo.end();
	fbo.pop();
		
	
		
	
	ofSetColor(0xffffff);
	
	tex.draw(0,0);
	if (!strokes.empty()) {
		for (vector<ofxLine >::iterator liter = strokes.begin(); liter!=strokes.end(); liter++) {
			liter->draw();
		}
	}
	
	
	ofSetColor(0xff0000);
	
	for (vector<ofPoint >::iterator piter = intersections.begin(); piter!=intersections.end(); piter++) {
		ofCircle(piter->x, piter->y, 2);
	}
	
	ofNoFill();
	for (vector<evil >::iterator eiter = evils.begin(); eiter!=evils.end(); eiter++) {
		ofCircle(eiter->cpos.x, eiter->cpos.y, 3);
	}
	
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	//grayPixels[(int)(touch.y*w+touch.x)] = 255;
	pnt = ofPoint(touch.x,touch.y);
	
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	
	//grayPixels[(int)(touch.y*w+touch.x)] = 255;
	ofPoint pnt2 = ofPoint(touch.x,touch.y);
	ofxLine line = ofxLine(pnt,pnt2);
	
	if (strokes.size()>1) {
		for (vector<ofxLine >::iterator liter = strokes.begin(); (liter+1)!=strokes.end(); liter++) {
			ofPoint p;
			if (line.getInterception(*liter,p)) {
				bConnected = false;
				strokes.clear();
				return;
				
			} 
		}
	}
	
		
	if (!bConnected) {
		for (vector<ofxLine >::iterator liter = lines.begin(); liter!=lines.end(); liter++) {
			ofPoint p;
			if (line.getInterception(*liter,p)) {
				bConnected = true;
				intersections.push_back(p);
				strokes.push_back(ofxLine(p,pnt2));
				break;
			}
		}
	} else {
		for (vector<ofxLine >::iterator liter = lines.begin(); liter!=lines.end(); liter++) {
			ofPoint p;
			if (line.getInterception(*liter,p)) {
				bConnected = false;
				intersections.push_back(p);
				strokes.push_back(ofxLine(pnt,p));
				break;
			} 
		}
		
		if (bConnected) {
			strokes.push_back(ofxLine(pnt,pnt2));
		} else {
			
	
			
			
			for (vector<ofxLine >::iterator liter = strokes.begin(); liter!=strokes.end(); liter++) {
				lines.push_back(*liter);
			}
			
			int i = strokes.size()/2;
			strokes[i].getCrossBorderPoints(fill1,fill2);
			strokes.clear();
			
			cout << "(" << fill1.x << "," << fill1.y << ") " << "(" << fill2.x << "," << fill2.y << ") " << endl;
			bFill = true;
			
			
		}

	}

	pnt=pnt2;
	
		
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	bConnected = false;
	strokes.clear();
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
	
	
	
}

void testApp::exit() {
	cout << "exit" <<endl;
}
