#include "testApp.h"

#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
	load();
	save();
	piter = params.begin();
	
	ofBackground(0, 0, 0);
	
	graph = ofRectangle(30, 30, 500, 200);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(0xFFFFFF);
	
	float dx = graph.width/(numSamples);
	
	ofPushMatrix();
	ofTranslate(graph.x, graph.y, 0);
	ofNoFill();
	ofBeginShape();
	for(vector<float>::iterator iter=piter->samples.begin(); iter!= piter->samples.end()	; iter++){
		float x = (distance(piter->samples.begin(),iter))*dx;
		ofVertex(x, graph.height*( 1- (*iter-piter->minValue)/(piter->maxValue-piter->minValue)));
	}
	ofEndShape(false);
	
	ofRect(0, 0, graph.width, graph.height);
	float y = graph.height*( 1- (0-piter->minValue)/(piter->maxValue-piter->minValue));
		
	//ofLine(0, y, graph.width, y);
	
	ofDrawBitmapString(piter->name, 0, graph.height+20);
	
	ofPopMatrix();

}


void testApp::load() {
	ofxXmlSettings xml;
	
	bool loaded = xml.loadFile("params.xml");
	
	assert(loaded);
	
	numSamples = xml.getAttribute("params", "numSamples", 10);
	
	xml.pushTag("params");
	
	for (int i=0;i<xml.getNumTags("param");i++) {
		param p;
		
		p.name = xml.getAttribute("param", "name", "noname", i);
		p.minValue = xml.getAttribute("param", "minValue", 0.0f, i);
		p.maxValue = xml.getAttribute("param", "maxValue", 1.0f, i);
		xml.pushTag("param", i);
		for (int j=0; j<xml.getNumTags("sample") && j<numSamples; j++) {
			p.samples.push_back(xml.getAttribute("sample", "value", 1.0f, j));
		}
		xml.popTag();
		
		if (p.samples.size() < numSamples) {
			for (int j=p.samples.size()-1; j<numSamples; j++) {
				p.samples.push_back(0);
			}
		}

		params.push_back(p);
	}
	
	xml.popTag();
	
}

void testApp::save() {
	dump();
	ofxXmlSettings xml;
	
	xml.addTag("params");
	xml.addAttribute("params", "numSamples", numSamples,0);
	
	xml.pushTag("params");
	
	for (int i=0;i<params.size();i++) {
		param p = params.at(i);
		xml.addTag("param");
		xml.addAttribute("param", "name", p.name, i);
		xml.addAttribute("param", "minValue", p.minValue, i);
		xml.addAttribute("param", "maxValue", p.maxValue, i);
		
		xml.pushTag("param", i);
		
		for (int j=0; j<p.samples.size(); j++) {
			xml.addTag("sample");
			xml.addAttribute("sample", "value", p.samples.at(j), j);
		}
		xml.popTag();
		
	}
	
	xml.popTag();
	
	xml.saveFile("params.xml");
	
	
}


void testApp::dump() {
	ofxXmlSettings xml;
	
	cout << numSamples << endl;
	
	for (int i=0;i<params.size();i++) {
		param p = params.at(i);
		
		cout << "name: " << p.name << ", minValue: " << p.minValue << ", maxValue: " << p.maxValue << " ";
				
		for (int j=0; j<p.samples.size(); j++) {
			cout << p.samples.at(j) << " ";
		}
		cout << endl;
		
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
		case '+':
			piter++;
			piter = piter==params.end() ? params.begin() : piter;
			save();
			break;
		case '-':
			piter = piter==params.begin() ? params.end() : piter;
			piter--;
			save();
			break;
		case 's':
			save();
			break;
		default:
			break;
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
	if (x>=graph.x && x<graph.x+graph.width && y>=graph.y && y<graph.y+graph.height) {
		float value = piter->minValue+(1-((float)y-graph.y)/graph.height)*(piter->maxValue - piter->minValue);
		int i = ((float)x-graph.x)/graph.width * numSamples;
		piter->samples.at(i) = value;
	}
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

