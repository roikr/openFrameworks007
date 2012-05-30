#include "testApp.h"
#include "ofxXmlSettings.h"

#define STAGE_WIDTH 2048
#define STAGE_HEIGHT 1024

#define BORDER_SIZE 2048
#define TILE_SIZE 200

document testApp::parseDoc(string name) {
    document doc;
    
    ofxXmlSettings xml;
    
    if (xml.loadFile(name)) {
        xml.pushTag("DOMDocument");
        xml.pushTag("media");
        for (int i=0; i<xml.getNumTags("DOMBitmapItem"); i++) {
            string name = xml.getAttribute("DOMBitmapItem", "name", "",i);
            int frameRight = xml.getAttribute("DOMBitmapItem", "frameRight", 0,i);
            int frameBottom = xml.getAttribute("DOMBitmapItem", "frameBottom", 0,i);
 //           cout << name << "\t" << frameRight/20 << "x" << frameBottom/20 << endl;
            doc.items[name]=make_pair(frameRight/20, frameBottom/20);
        }
        xml.popTag();
        xml.popTag();
        
    }
    
    return doc;
}

vector<ofVec2f> parseVec(string str) {
    vector<ofVec2f> vec;
    vector<string> svec=ofSplitString(str, " ");
    for (vector<string>::iterator iter=svec.begin(); iter!=svec.end(); iter+=2) {
        vec.push_back(ofVec2f(ofToFloat(*iter),ofToFloat(*(iter+1))));
    }
    return vec;
}

symbolItem testApp::parseSymbol(string name) {
    
    symbolItem item;
    
    ofxXmlSettings xml;
    
    if (xml.loadFile("LIBRARY/"+name)) {
        xml.pushTag("DOMSymbolItem");
        xml.pushTag("timeline");
        xml.pushTag("DOMTimeline");
        xml.pushTag("layers");
        xml.pushTag("DOMLayer");
        xml.pushTag("frames");
        xml.pushTag("DOMFrame");
        xml.pushTag("elements");
        for (int i=0; i<xml.getNumTags("DOMBitmapInstance"); i++) {
            bitmapInstance bi;
            bi.libraryItemName = xml.getAttribute("DOMBitmapInstance", "libraryItemName", "",i);
            bi.u = 1;
            bi.v = 1;
            xml.pushTag("DOMBitmapInstance",i);
            
            
            xml.pushTag("matrix");
            bi.translation = ofVec2f(xml.getAttribute("Matrix", "tx", 0.0),xml.getAttribute("Matrix", "ty", 0.0));
            bi.scale = xml.getAttribute("Matrix", "a", 0.0);
            bi.rotation = xml.getAttribute("Matrix", "c", 0.0);
            xml.popTag();
            
            bi.bHasTransformationPoint = xml.tagExists("transformationPoint");
            if (bi.bHasTransformationPoint) {
                xml.pushTag("transformationPoint");
                bi.transformationPoint = ofVec2f(xml.getAttribute("Point", "x", 0.0),xml.getAttribute("Point", "y", 0.0));
                xml.popTag();
            }
            
            xml.popTag();
            item.bitmaps.push_back(bi);
        }
        
        for (int i=0; i<xml.getNumTags("DOMShape"); i++) {
            shape s;
            
            xml.pushTag("DOMShape",i);
            xml.pushTag("paths");
            
            string str = xml.getAttribute("Path", "data", "");
            
            size_t found = str.find('L');
            if (found!=string::npos) {
                line l;
                l.p0 = parseVec(str.substr(1,found)).front();
                l.p1 = parseVec(str.substr(found+1)).front();
                s.lines.push_back(l);
            } else {
                found = str.find('C');
                if (found!=string::npos) {
                    curve c;
                    c.p0 = parseVec(str.substr(1,found)).front();
                    
                    vector<string> svec = ofSplitString(str.substr(found+1), "C");
                    cout << "numCurves: " << svec.size() << endl;
                    
                    for (vector<string>::iterator iter=svec.begin(); iter!=svec.end(); iter++) {
                        vector<ofVec2f> bezier = parseVec(*iter);
                        
                        cout << bezier.size() << "\t";
                        c.beziers.push_back(bezier);
                    }
                    
                    s.curves.push_back(c);
                    
                    
                }
            }
            
            xml.popTag();           
            xml.popTag();
            item.shapes.push_back(s);
        }
        
        xml.popTag();
        xml.popTag();
        xml.popTag();
        xml.popTag(); 
        xml.popTag();
        xml.popTag();
        xml.popTag();
        xml.popTag();
    }
    
    return item;
}

void testApp::loadSymbol(symbolItem &item,document &doc) {
    for (vector<bitmapInstance>::iterator iter=item.bitmaps.begin(); iter!=item.bitmaps.end(); iter++) {
        ofFile file = ofFile("LIBRARY/"+iter->libraryItemName);
//        iter->image.loadImage(file.getEnclosingDirectory()+file.getBaseName()+".pvr");
//        iter->image.update();
        iter->texture.load(file.getEnclosingDirectory()+file.getBaseName()+".pvr");
    
        int width = doc.items[iter->libraryItemName].first;
        int height = doc.items[iter->libraryItemName].second;
        iter->u = (float)width/(float)iter->texture._width;
        iter->v = (float)height/(float)iter->texture._height;
//        cout << iter->libraryItemName << "\t" << iter->u << "x" << iter->v << endl;
        if (!iter->bHasTransformationPoint) {
            iter->transformationPoint = ofVec2f(width/2,height/2);//ofVec2f(iter->image.width/2,iter->image.height/2);
        }
    }
}

void testApp::drawSymbol(symbolItem &item) {
    for (vector<bitmapInstance>::iterator iter=item.bitmaps.begin(); iter!=item.bitmaps.end(); iter++) {
        ofPushMatrix();
        ofTranslate(iter->translation);
        ofScale(iter->scale, iter->scale);
        iter->texture.draw(iter->u,iter->v);
        ofPopMatrix();
    }
    
    for (vector<shape>::iterator siter=item.shapes.begin(); siter!=item.shapes.end(); siter++) {
        for (vector<line>::iterator liter=siter->lines.begin(); liter!=siter->lines.end(); liter++) {
            ofLine(liter->p0, liter->p1);
        }
        for (vector<curve>::iterator citer=siter->curves.begin(); citer!=siter->curves.end(); citer++) {
            ofBeginShape();
            ofVertex(citer->p0.x,citer->p0.y);
            for (vector<vector<ofVec2f> >::iterator iter=citer->beziers.begin(); iter!=citer->beziers.end(); iter++) {
               
                ofBezierVertex(iter->at(0).x,iter->at(0).y,iter->at(1).x,iter->at(1).y,iter->at(2).x,iter->at(2).y);
            }
            ofEndShape();
        }
        
    }

}

void testApp::releaseSymbol(symbolItem &item) {
    for (vector<bitmapInstance>::iterator iter=item.bitmaps.begin(); iter!=item.bitmaps.end(); iter++) {
        iter->texture.release();
    }
    
}

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofRegisterTouchEvents(this);
	
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_LEFT);
    
    doc = parseDoc("DOMDocument.xml");
    renders = parseSymbol("renders.xml");
    references = parseSymbol("references.xml");
    shapes = parseSymbol("Spread_2.xml");
    loadSymbol(renders,doc);
    loadSymbol(references,doc);
    
    for (int i=0; i<references.bitmaps.size(); i++) {
        zOffsets.push_back(ofRandom(-20, 20));
    }
        
    cam.setZoom(1.0f);
	cam.setMinZoom(0.4f);
	cam.setMaxZoom(7.5f);
	cam.setScreenSize( ofGetWidth(), ofGetHeight() );
//    cout << ofGetWidth() << "," << ofGetHeight() << endl;
	cam.setViewportConstrain( ofVec3f(-(STAGE_WIDTH/2+BORDER_SIZE), -(STAGE_HEIGHT/2+BORDER_SIZE)), ofVec3f(STAGE_WIDTH/2+BORDER_SIZE, STAGE_HEIGHT/2+BORDER_SIZE)); //limit browseable area, in world units
	
    
    ofEnableAlphaBlending();
//    ofBackgroundHex(0x303030);
    ofBackgroundHex(0x919191);
    
    int bufferSize = 256;
	int sampleRate = 44100;
    ofSoundStreamSetup(2, 0, this, sampleRate, bufferSize, 4);	
    ofSetFrameRate(60);
	
}


//--------------------------------------------------------------
void testApp::update(){
   player.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    
    cam.apply();
    
  
    
//    int rotatedTileSize = TILE_SIZE*sin(PI/4);
    int numCols = (STAGE_WIDTH+2*BORDER_SIZE)/TILE_SIZE;///rotatedTileSize;
    int numRows = (STAGE_HEIGHT+2*BORDER_SIZE)/TILE_SIZE;///rotatedTileSize;
    
    ofSetHexColor(0xC0AAA2);
//    ofSetHexColor(0x320006);
    ofPushMatrix();
    glTranslatef(0, 0, -200);
    ofFill();
    for (int j=-numRows;j<=numRows;j++) {
        for (int i=-numCols;i<=numCols;i++) {
            if ((i+j) %2) {
                ofRect(i*TILE_SIZE, j*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }
        }
    }
    ofPopMatrix();
    
    ofPushMatrix();
    
    ofTranslate(-STAGE_WIDTH/2, -STAGE_HEIGHT/2);
    
    
    float zoomFactor = ofMap(cam.zoom,0.8,2,0,1,true);
    
    for (vector<bitmapInstance>::iterator iter=references.bitmaps.begin(); iter!=references.bitmaps.end(); iter++) {
        ofVec2f center =  ofVec2f(STAGE_WIDTH/2,STAGE_HEIGHT/2)-cam.offset;
        float distFactor = ofMap(iter->translation.distance(center), 200, 400, 1, 0,true);
        
        ofSetColor(255, 255, 255, 255*ofMap(zoomFactor*distFactor, 0, 1, 0.05, 1));
        ofPushMatrix();
        glTranslatef(iter->translation.x, iter->translation.y, zOffsets[distance(references.bitmaps.begin(), iter)]);
//        ofTranslate(iter->translation);
        ofScale(iter->scale, iter->scale);
        iter->texture.draw(iter->u,iter->v);
        ofPopMatrix();
    }

    
   
    ofSetColor(255, 255, 255, 255);
    ofPushMatrix();
    glTranslatef(0, 0, -100);
    drawSymbol(renders);
    ofPopMatrix();
    
    ofNoFill();
    drawSymbol(shapes);
    
    ofPopMatrix();
    
    int width = 200;
    int height = width*player.getHeight()/player.getWidth();
    player.draw(ofRectangle(videoPos.x-width/2,videoPos.y-height/2,width,height),ofRectangle(0,0,1,1));
    
    cam.reset();
    
    
    
    cam.drawDebug(); //see info on ofxPanZoom status
	
	glColor4f(1,1,1,1);
	ofDrawBitmapString("fps: " + ofToString( ofGetFrameRate(), 1 ),  10, ofGetHeight() - 10 );	
    ofDrawBitmapString("zoomFactor: " + ofToString( zoomFactor, 2 ),  10, ofGetHeight() - 30 );	
}

//--------------------------------------------------------------
void testApp::audioOut( float * output, int bufferSize, int nChannels ){
    
	player.audioRequested(output, bufferSize, nChannels);
	
}


//--------------------------------------------------------------
void testApp::exit(){
    releaseSymbol(renders);
    releaseSymbol(references);
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
    cam.touchDown(touch);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
    cam.touchMoved(touch);

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
    cam.touchUp(touch);

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs &touch){
    cam.touchDoubleTap(touch);
//    cam.setZoom(1.0f);	//reset zoom
//	cam.lookAt( ofVec3f() ); //reset position
    videoPos = cam.screenToWorld(ofVec2f(touch.x,touch.y));
    player.play(ofToDataPath("piezo_pinDrop.mp4"));

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

