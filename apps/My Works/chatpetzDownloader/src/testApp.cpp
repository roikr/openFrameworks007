#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);

	assert(xml.loadFile("downloader.xml"));
	
	xml.pushTag("downloader");
	background.loadImage(xml.getAttribute("background", "filename", ""));
	progressBar.loadImage(xml.getAttribute("progress", "bar", ""));
	progressBackground.loadImage(xml.getAttribute("progress", "background", ""));
	progressPos = ofPoint(xml.getAttribute("progress", "x", 0),xml.getAttribute("progress", "y", 0));
	titleOffset = ofPoint(xml.getAttribute("title_offset", "x", 0),xml.getAttribute("title_offset", "y", 0));
	ttf.loadFont(xml.getAttribute("font", "filename", ""), xml.getAttribute("font", "size", 20));
	normal.loadImage(xml.getAttribute("button", "normal", ""));
	highlighted.loadImage(xml.getAttribute("button", "highlighted", ""));
	selected.loadImage(xml.getAttribute("button", "selected", ""));
	
	for (int i=0; i<xml.getNumTags("bundle"); i++) {
		bundle b;
		b.path = xml.getAttribute("bundle", "path", "", i);
		b.title = xml.getAttribute("bundle", "title", "", i);
		b.pos = ofPoint(xml.getAttribute("bundle", "x", 0, i),xml.getAttribute("bundle", "y", 0, i));
		
		if (xml.attributeExists("bundle", "normal", i)) {
			b.normal = new ofImage;
			b.normal->loadImage(xml.getAttribute("bundle", "normal", "", i));
		} else {
			b.normal = &normal;
		}
		
		if (xml.attributeExists("bundle", "highlighted", i)) {
			b.highlighted = new ofImage;
			b.highlighted->loadImage(xml.getAttribute("bundle", "highlighted", "", i));
		} else {
			b.highlighted = &highlighted;
		}
		
		if (xml.attributeExists("bundle", "selected", i)) {
			b.selected = new ofImage;
			b.selected->loadImage(xml.getAttribute("bundle", "selected", "", i));
		} else {
			b.selected = &selected;
		}
		
		b.state = STATE_NORMAL;
		
		bundles.push_back(b);
	}
	
	xml.popTag();
	
	progress= 0.5;
}

//--------------------------------------------------------------
void testApp::update(){
	//we change the background color here based on the values
	//affected by the mouse position
	ofBackground(255,255,255);

}

//----------------------------------------------------------
void testApp::drawTexture(ofTextureData &texData, float x, float y, float w, float h){
	
	glEnable(texData.textureTarget);
	
	// bind the texture
	glBindTexture( texData.textureTarget, (GLuint)texData.textureID );
	
	GLfloat px0 = 0;		// up to you to get the aspect ratio right
	GLfloat py0 = 0;
	GLfloat px1 = w;
	GLfloat py1 = h;
	
		
	// -------------------------------------------------
	
	GLfloat tx0 = 0;
	GLfloat ty0 = 0;
//	GLfloat tx1 = texData.tex_t - offsetw;
//	GLfloat ty1 = texData.tex_u - offseth;
	GLfloat tx1 = w;
	GLfloat ty1 = h;
	
	glPushMatrix(); 
	
	glTranslatef(x,y,0.0f);
	
	GLfloat tex_coords[] = {
		tx0,ty0,
		tx1,ty0,
		tx1,ty1,
		tx0,ty1
	};
	GLfloat verts[] = {
		px0,py0,
		px1,py0,
		px1,py1,
		px0,py1
	};
	
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(2, GL_FLOAT, 0, verts );
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glPopMatrix();
	glDisable(texData.textureTarget);
}


//--------------------------------------------------------------
void testApp::draw(){

	//---------
	//Lets draw the stroke as a continous line
	ofSetColor(0xFFFFFF);
	background.draw(0, 0);
	ofEnableAlphaBlending();
	progressBackground.draw(progressPos.x, progressPos.y);
//	progressBar.draw(progressPos.x, progressPos.y);
	drawTexture(progressBar.getTextureReference().texData, progressPos.x, progressPos.y, progressBar.width*progress, progressBar.height);
		
//
//	//instructions at the bottom
//	ttf.drawString("mouse drag changes background color and records stroke", 168, ofGetHeight() - 9);
	
	for (vector<bundle>::iterator iter=bundles.begin(); iter!=bundles.end(); iter++) {
		ofSetColor(0xFFFFFF);
		switch (iter->state) {
			case STATE_NORMAL:
				iter->normal->draw(iter->pos.x,iter->pos.y);
				break;
			case STATE_HIGHLIGHTED:
				iter->highlighted->draw(iter->pos.x,iter->pos.y);
				break;
			case STATE_SELECTED:
				iter->selected->draw(iter->pos.x,iter->pos.y);
				break;
			default:
				break;
		}
		
		ofSetColor(0x000000);
		ttf.drawString(iter->title, iter->pos.x+titleOffset.x, iter->pos.y+titleOffset.y);

	}
	ofDisableAlphaBlending();
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	for (vector<bundle>::iterator iter=bundles.begin(); iter!=bundles.end(); iter++) {
		if (iter->state!=STATE_SELECTED) {
			iter->state = (x >= iter->pos.x && x <= iter->pos.x+iter->normal->width && y >= iter->pos.y && y<=iter->pos.y+iter->normal->height) ? STATE_HIGHLIGHTED : STATE_NORMAL;
		}	
	}
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	vector<bundle>::iterator iter,siter;
	
	for (iter=bundles.begin(); iter!=bundles.end(); iter++) {
		if (x >= iter->pos.x && x <= iter->pos.x+iter->normal->width && y >= iter->pos.y && y<=iter->pos.y+iter->normal->height) {
			iter->state = STATE_SELECTED; 
			break;
		}
	}
	
	siter = iter;
	
	if (siter!=bundles.end()) {
		for (iter=bundles.begin(); iter!=bundles.end(); iter++) {
			if (iter!=siter) {
				iter->state = STATE_NORMAL;
			}
		}
	}
		
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
	
}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}



