#include "testApp.h"
#include "ofxXmlSettings.h"
#include "ofxFlash.h"

//--------------------------------------------------------------
void testApp::setup(){
        
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofBackground(255);
    
    ofSetDataPathRoot("/Users/roikr/Developer/of_preRelease_v007_iphone/apps/saveAs/saveAs/bin/data/");
    
    
    
    ofxSymbolItem layout;
    ofxDocument doc;
    
    doc.setup("DOMDocument.xml");
    layout.setup("Spread_1.xml",doc);
    
    ofxXmlSettings docXml;
    ofxXmlSettings xml;
    
    docXml.loadFile("DOMDocument.xml");
    xml.loadFile("LIBRARY/Spread_1.xml");
    
    
    ofImage image;
    image.setUseTexture(false);
    
    docXml.pushTag("DOMDocument");
    docXml.pushTag("media");
    
    xml.pushTag("DOMSymbolItem");
    xml.pushTag("timeline");
    xml.pushTag("DOMTimeline");
    xml.pushTag("layers");

    
    for (vector<layer>::iterator liter=layout.layers.begin();liter!=layout.layers.end();liter++) {
        
        cout << liter->name << endl;
        xml.pushTag("DOMLayer",distance(layout.layers.begin(), liter));
        xml.pushTag("frames");
        xml.pushTag("DOMFrame");
        xml.pushTag("elements");
        
        for (vector<shape>::iterator siter=liter->shapes.begin(); siter!=liter->shapes.end(); siter++) {
            if (!siter->bitmapFill.empty()) {
                vector<bitmap>::iterator iter = siter->bitmapFill.begin();
                
                
                image.loadImage("LIBRARY/"+iter->href);
                bitmapItem bi = doc.items[iter->path];
                
                if (bi.frameRight/PIXEL_SCALE != image.width || bi.frameBottom/PIXEL_SCALE != image.height) {
                    cout << bi.href << " " << bi.frameRight/PIXEL_SCALE << "x" << bi.frameBottom/PIXEL_SCALE << " " << image.width << "x" << image.height <<endl;
                    
                    
                    float sx = (float)bi.frameRight/PIXEL_SCALE/(float)image.width;
                    float sy = (float)bi.frameBottom/PIXEL_SCALE/(float)image.height;
                    
                    for (int i=0; i<docXml.getNumTags("DOMBitmapItem"); i++) {
                        if (docXml.getAttribute("DOMBitmapItem", "name", "",i) == iter->path) {
                            docXml.setAttribute("DOMBitmapItem", "frameRight", image.width*(int)PIXEL_SCALE,i);
                            docXml.setAttribute("DOMBitmapItem", "frameBottom", image.height*(int)PIXEL_SCALE,i);
                            break;
                        }
                        
                    }
                    
                    
                    xml.pushTag("DOMShape",distance(liter->shapes.begin(), siter));
                    xml.pushTag("paths");
                    xml.pushTag("Path");
                    xml.pushTag("fill");
                    xml.pushTag("BitmapFill");
                    xml.pushTag("matrix");
                    
                    float a = xml.getAttribute("Matrix", "a", 1.0);
                    float d = xml.getAttribute("Matrix", "d", 1.0);
                    
                    xml.setAttribute("Matrix", "a",a*sx,0);
                    xml.setAttribute("Matrix", "d",d*sy,0);
                    
                    cout << a << " " << sx << " " << d << " " << sy << endl;

                    
                    xml.popTag();
                    xml.popTag();
                    xml.popTag();
                    xml.popTag();
                    xml.popTag();
                    xml.popTag();
                }
                
            }
            
        }
        
        xml.popTag();
        xml.popTag();
        xml.popTag();
        xml.popTag();
        
    }
    
    xml.popTag();
    xml.popTag();
    xml.popTag();
    xml.popTag();
    
    docXml.popTag();
    docXml.popTag();
    
//    string docStr;
//    docXml.copyXmlToString(docStr);
    
//    cout << docStr << endl;
    
    docXml.saveFile("DOMDocument.xml");
    xml.saveFile("LIBRARY/Spread_1.xml");
    
    std::exit(0);
    
   
    

}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
    
 

    
}

//--------------------------------------------------------------
void testApp::exit(){
   
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}