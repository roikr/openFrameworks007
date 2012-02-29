#include "testApp.h"
#include "ofxXmlSettings.h"

#define CENTIMETERS_TO_PIXELS_RATIO 10.0

template <typename T> int sgn(T val)
{
    return (val > T(0)) - (val < T(0));
}

//--------------------------------------------------------------
void testApp::setup(){	
	ofBackground(0,0,0);	
	ofSetFrameRate(60);
    
    src.push_back(ofVec3f(0,0));
    src.push_back(ofVec3f(ofGetWidth(),0));
    src.push_back(ofVec3f(ofGetWidth(),ofGetHeight()));
    src.push_back(ofVec3f(0,ofGetHeight()));
    
    font.loadFont("Guttman Hatzvi-Normal.ttf", 120,true,true);
    
    for (vector<ofVec3f>::iterator iter=src.begin(); iter!=src.end(); iter++) {
        draggableVertex dv;
        dv.pos = *iter;
        dv.bOver 			= false;
		dv.bBeingDragged 	= false;
		dv.radius = 10;
        
        vertices.push_back(dv);
        cityQuad.vertices.push_back(vertices.size()-1);
    }
	
    calcQuad(cityQuad);

    
    ofxXmlSettings xml;
    xml.loadFile("tents.xml");
    xml.pushTag("tents");
    for (int i=0;i<xml.getNumTags("base");i++) {
        xml.pushTag("base",i);
        base b;
        for (int j=0;j<xml.getNumTags("vertex");j++) {
            anchors.push_back(ofVec3f(xml.getAttribute("vertex", "x", 0.0,j)*CENTIMETERS_TO_PIXELS_RATIO,xml.getAttribute("vertex", "y", 0.0,j)*CENTIMETERS_TO_PIXELS_RATIO));
            b.vertices.push_back(anchors.size()-1);
        }
        bases.push_back(b);
        xml.popTag();
    }
    
    for (vector<base>::iterator biter=bases.begin(); biter!=bases.end(); biter++) {
        for (vector<int>::iterator viter=biter->vertices.begin(); viter!=biter->vertices.end(); viter++) {
            vector<int>::iterator niter = viter+1;
            if (niter==biter->vertices.end()) {
                niter=biter->vertices.begin();
            }
            
            float a0 = M_PI*(2.0*distance(biter->vertices.begin(), viter)-3.0)/4.0;
            float a1 = M_PI*(2.0*distance(biter->vertices.begin(), niter)-3.0)/4.0;
            float k = 10;
            
            vector<ofVec3f> pts;
            
            ofVec3f vec2 = cityQuad.matrix.postMult(anchors[*viter]);
            ofVec3f vec3 = cityQuad.matrix.postMult(anchors[*niter]);
            
            pts.push_back(ofVec3f(vec3.x-k*sgn(cos(a1)),vec3.y-k*sgn(sin(a1))));
            pts.push_back(ofVec3f(vec2.x-k*sgn(cos(a0)),vec2.y-k*sgn(sin(a0))));
            pts.push_back(vec2);
            pts.push_back(vec3);
            
            
            quad q;
            
            for (vector<ofVec3f>::iterator piter=pts.begin(); piter!=pts.end(); piter++) {
                draggableVertex dv;
                dv.pos = *piter;
                dv.bOver 			= false;
                dv.bBeingDragged 	= false;
                dv.radius = 10;
                
                vertices.push_back(dv);
                q.vertices.push_back(vertices.size()-1);
            }
            
            calcQuad(q);
            quads.push_back(q);
            quadsAnchors.push_back(*viter);
        }
    }
    
    bJustText = false;
    message = L"םילהוא בוט רקוב";
    messageWidth = font.stringWidth(message);
    messageHeight = font.stringHeight(message);
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
		
    ofEnableSmoothing();
	
	// show a faint the non-curve version of the same polygon:
	ofEnableAlphaBlending();
    
    
    if (!bJustText) {
        ofFill();
        ofSetColor(255,255,255,50);
        ofBeginShape();
        for (vector<int>::iterator iter = cityQuad.vertices.begin(); iter!=cityQuad.vertices.end() ; iter++){
            ofVertex(vertices[*iter].pos.x, vertices[*iter].pos.y);
        }
        ofEndShape(true);
        
        
        ofSetColor(255,255,255,200);
        
        for (vector<draggableVertex>::iterator iter=vertices.begin(); iter!=vertices.end(); iter++) {
            if (iter->bOver == true) ofFill();
            else ofNoFill();
            ofCircle(iter->pos.x, iter->pos.y,4);
        }
        
		
        
        //-------------------------------------
        
        
	    
        glPushMatrix();
        glMultMatrixf(cityQuad.glMatrix.getPtr());
        glPushMatrix();
        
        
        
        ofSetColor(255,255,0,50);
        ofFill();
        
        for (vector<base>::iterator biter=bases.begin(); biter!=bases.end(); biter++) {
            ofBeginShape();
            for (vector<int>::iterator viter=biter->vertices.begin(); viter!=biter->vertices.end(); viter++) {
                ofVertex(anchors[*viter].x, anchors[*viter].y);
            }
            ofEndShape(true);
        }
        
        
        ofSetColor(255,255,255,255);
        ofFill();
        //wstring str = 
        
        //font.drawString(L"הקידב", 200, 200);
        //font.drawStringAsShapes(L"םילהוא בוט רקוב", 10, 700);
        //font.drawString(L"םילהוא בוט רקוב", 10, 700);
        
        
        
        glPopMatrix();
        glPopMatrix(); 
    }
       
    
    
    
    for (vector<quad>::iterator qiter=quads.begin(); qiter!=quads.end(); qiter++) {
        glPushMatrix();
        glMultMatrixf(qiter->glMatrix.getPtr());
        glPushMatrix();
        
        
        ofFill();
		ofSetColor(255,255,255,50);
        
        if (!bJustText) {
            ofBeginShape();
            for (vector<ofVec3f>::iterator iter = src.begin(); iter!=src.end() ; iter++){
                ofVertex(iter->x, iter->y);
            }
            ofEndShape(true);
        }
		        
        
        font.drawString(message, (ofGetWidth()-messageWidth)/2, ofGetHeight()-messageHeight);
        
        
        glPopMatrix();
        glPopMatrix(); 
    }
    
    
    
//    ofMatrix4x4 mvMatrix;
//    glGetFloatv(GL_MODELVIEW_MATRIX,mvMatrix.getPtr());
    
//    ofMatrix4x4 homography = ofMatrix4x4::getTransposedOf(mvMatrix);
//    homography.makeIdentityMatrix();
    
    //homography.preMult(ofMatrix4x4(matrix));
 
    /*
    ofMatrix4x4 homography = ofMatrix4x4::getTransposedOf(ofMatrix4x4(matrix)); // openGL is transposed to ofMatrix4x4
    
    ofSetColor(0,255,0,50);
    ofFill();
    
    for (vector<base>::iterator biter=bases.begin(); biter!=bases.end(); biter++) {
        ofBeginShape();
        for (vector<int>::iterator viter=biter->vertices.begin(); viter!=biter->vertices.end(); viter++) {
            ofVec3f vertex = homography.postMult(anchors[*viter]);
            ofVertex(vertex.x, vertex.y);
        }
        ofEndShape(true);
    }
    */
    
   ofDisableSmoothing();
    ofDisableAlphaBlending();
     
}


	
	
void testApp::calcQuad(quad &q) {
    
    vector<ofVec3f> dst;
    for (vector<int>::iterator iter = q.vertices.begin(); iter!=q.vertices.end(); iter++) {
        dst.push_back(vertices[*iter].pos);
    }
    
    findHomography(src.data(), dst.data(), q.glMatrix.getPtr());
    q.matrix = ofMatrix4x4::getTransposedOf(q.glMatrix);
                          
}

void testApp::calcTents() {
    calcQuad(cityQuad);
    
    for (vector<base>::iterator biter=bases.begin(); biter!=bases.end(); biter++) {
        
        for (vector<int>::iterator viter=biter->vertices.begin(); viter!=biter->vertices.end(); viter++) {
            vector<int>::iterator niter = viter+1;
            if (niter==biter->vertices.end()) {
                niter=biter->vertices.begin();
            }
            
            for (vector<int>::iterator aiter=quadsAnchors.begin(); aiter!=quadsAnchors.end(); aiter++) {
                if (*aiter==*viter) {
                    quad &q = quads[distance(quadsAnchors.begin(),aiter)];
                    vertices[q.vertices[2]].pos = cityQuad.matrix.postMult(anchors[*viter]);
                    vertices[q.vertices[3]].pos = cityQuad.matrix.postMult(anchors[*niter]);
                    calcQuad(q);
                }
            }
            
        }
    }

    
    
    
    
}


void testApp::findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]){
	// arturo castro - 08/01/2010
	//
	// create the equation system to be solved
	//
	// from: Multiple View Geometry in Computer Vision 2ed
	//       Hartley R. and Zisserman A.
	//
	// x' = xH
	// where H is the homography: a 3 by 3 matrix
	// that transformed to inhomogeneous coordinates for each point
	// gives the following equations for each point:
	//
	// x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
	// y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
	//
	// as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
	// so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
	// after ordering the terms it gives the following matrix
	// that can be solved with gaussian elimination:
	
	float P[8][9]={
		{-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
		{  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12
		
		{-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
		{  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21
		
		{-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
		{  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23
		
		{-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
		{  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
	};
	
	gaussian_elimination(&P[0][0],9);
	
	// gaussian elimination gives the results of the equation system
	// in the last column of the original matrix.
	// opengl needs the transposed 4x4 matrix:
	float aux_H[]={ P[0][8],P[3][8],0,P[6][8], // h11  h21 0 h31
		P[1][8],P[4][8],0,P[7][8], // h12  h22 0 h32
		0 ,      0,0,0,       // 0    0   0 0
		P[2][8],P[5][8],0,1};      // h13  h23 0 h33
	
	for(int i=0;i<16;i++) homography[i] = aux_H[i];
}

void testApp::gaussian_elimination(float *input, int n){
	// arturo castro - 08/01/2010
	//
	// ported to c from pseudocode in
	// http://en.wikipedia.org/wiki/Gaussian_elimination
	
	float * A = input;
	int i = 0;
	int j = 0;
	int m = n-1;
	while (i < m && j < n){
		// Find pivot in column j, starting in row i:
		int maxi = i;
		for(int k = i+1; k<m; k++){
			if(fabs(A[k*n+j]) > fabs(A[maxi*n+j])){
				maxi = k;
			}
		}
		if (A[maxi*n+j] != 0){
			//swap rows i and maxi, but do not change the value of i
			if(i!=maxi)
				for(int k=0;k<n;k++){
					float aux = A[i*n+k];
					A[i*n+k]=A[maxi*n+k];
					A[maxi*n+k]=aux;
				}
			//Now A[i,j] will contain the old value of A[maxi,j].
			//divide each entry in row i by A[i,j]
			float A_ij=A[i*n+j];
			for(int k=0;k<n;k++){
				A[i*n+k]/=A_ij;
			}
			//Now A[i,j] will have the value 1.
			for(int u = i+1; u< m; u++){
				//subtract A[u,j] * row i from row u
				float A_uj = A[u*n+j];
				for(int k=0;k<n;k++){
					A[u*n+k]-=A_uj*A[i*n+k];
				}
				//Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
			}
			
			i++;
		}
		j++;
	}
	
	//back substitution
	for(int i=m-2;i>=0;i--){
		for(int j=i+1;j<n-1;j++){
			A[i*n+m]-=A[i*n+j]*A[j*n+m];
			//A[i*n+j]=0;
		}
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 't':
            bJustText = !bJustText;
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//------------- -------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	for (vector<draggableVertex>::iterator iter=vertices.begin(); iter!=vertices.end(); iter++) {
        if (iter->pos.distance(ofVec3f(x,y)) < iter->radius){
			iter->bOver = true;
		} else {
			iter->bOver = false;
		}	 
    }
    
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	for (vector<draggableVertex>::iterator iter=vertices.begin(); iter!=vertices.end(); iter++) {
        if (iter->bBeingDragged == true){
			iter->pos.x = x;
			iter->pos.y = y;
            
            for (vector<int>::iterator viter=cityQuad.vertices.begin(); viter!=cityQuad.vertices.end(); viter++) {
                if (*viter == distance(vertices.begin(), iter)) {
                    calcTents();
                    break;
                }
            }
            
            for (vector<quad>::iterator qiter=quads.begin(); qiter!=quads.end(); qiter++) {
                for (vector<int>::iterator viter=qiter->vertices.begin(); viter!=qiter->vertices.end(); viter++) {
                    if (*viter == distance(vertices.begin(), iter)) {
                        calcQuad(*qiter);
                        break;
                    }
                }
            }
            
            
		}
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	for (vector<draggableVertex>::iterator iter=vertices.begin(); iter!=vertices.end(); iter++) {
        if (iter->pos.distance(ofVec3f(x,y)) < iter->radius){
			iter->bBeingDragged = true;
            break;
		} else {
			iter->bBeingDragged = false;
		}	 
    }
    
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    for (vector<draggableVertex>::iterator iter=vertices.begin(); iter!=vertices.end(); iter++) {
        iter->bBeingDragged = false;
    }
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

