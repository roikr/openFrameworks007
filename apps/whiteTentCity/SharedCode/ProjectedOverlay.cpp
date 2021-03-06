//
//  ProjectedOverlay.cpp
//  tentsMapping
//
//  Created by Roee Kremer on 3/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ProjectedOverlay.h"
#include <iostream>
#include "ofxXmlSettings.h"


void readVertex(ofxXmlSettings &xml,ofVec3f &vertex,int i) {
    vertex = ofVec3f(xml.getAttribute("vertex", "x", 0.0f,i),xml.getAttribute("vertex", "y", 0.0f,i),xml.getAttribute("vertex", "z", 0.0f,i));
}

void readScreen(ofxXmlSettings& xml, screen &s,int which) {
    s.index = xml.getAttribute("screen", "index",0, which);
    
    xml.pushTag("screen",which);
    
    for (int j=0;j<xml.getNumTags("index");j++) {
        s.indices.push_back(xml.getAttribute("index", "value", 0,j));
        
    }
    
    readVertex(xml, s.origin,0);
    readVertex(xml,s.xVec, 1);
    readVertex(xml,s.yVec, 2);
    
    
    xml.popTag();
}

void ProjectedOverlay::load() {
    ofxXmlSettings xml;
    xml.loadFile(filename);
    xml.pushTag("city");
    
    for (int i=0;i<xml.getNumTags("screen"); i++) {
        screen s;
        readScreen(xml, s, i);
        screens.push_back(s);
    }
    
    for (int i=0;i<xml.getNumTags("tent");i++) {
        tent t;
        t.id = xml.getAttribute("tent", "id", 100+i,i);
        xml.pushTag("tent",i);
        
        for (int j=0;j<xml.getNumTags("index");j++) {
            t.indices.push_back(xml.getAttribute("index", "value", 0,j));
        }
        
        for (int j=0;j<xml.getNumTags("vertex");j++) {
            ofVec3f vertex;
            readVertex(xml, vertex,j);
            t.vertices.push_back(vertex);
            
        }
        
        
        tents.push_back(t);
        xml.popTag();
    }
    
    if (xml.tagExists("playground")) {
        xml.pushTag("playground");
        readScreen(xml, playground, 0);
        for (int i=0;i<xml.getNumTags("polygon");i++) {
            polygon p;
            
            xml.pushTag("polygon",i);
            
            //        for (int j=0;j<xml.getNumTags("index");j++) {
            //            p.indices.push_back(xml.getAttribute("index", "value", 0,j));
            //        }
            
            for (int j=0;j<xml.getNumTags("vertex");j++) {
                ofVec3f vertex;
                readVertex(xml, vertex,j);
                p.vertices.push_back(vertex);
                
            }
            
            
            polygons.push_back(p);
            xml.popTag();
        }
        xml.popTag();
    }
    
    
    
    
    xml.popTag();
    
    updateMatrices();
}

void writeVertex(ofxXmlSettings &xml,ofVec3f &vertex,int which) {
    xml.addTag("vertex");
    xml.addAttribute("vertex", "x", vertex.x,which);
    xml.addAttribute("vertex", "y", vertex.y,which);
    xml.addAttribute("vertex", "z", vertex.z,which);
}

void writeScreen(ofxXmlSettings& xml, screen &s,int which) {
    xml.addTag("screen");
    xml.addAttribute("screen", "index", s.index,0);
    xml.pushTag("screen",which);
    for (vector<int>::iterator iter=s.indices.begin();iter!=s.indices.end();iter++) {
        xml.addTag("index");
        xml.addAttribute("index", "value", *iter,distance(s.indices.begin(), iter));
    }
    
    writeVertex(xml, s.origin,0);
    writeVertex(xml, s.xVec,1);
    writeVertex(xml, s.yVec,2);
    
    
    
    xml.popTag();
}

void ProjectedOverlay::save() {
    ofxXmlSettings xml;
    
    xml.addTag("city");
    xml.pushTag("city");
    
    for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
        writeScreen(xml, *siter,distance(screens.begin(), siter));
    }
    
    for (vector<tent >::iterator titer=tents.begin(); titer!=tents.end(); titer++) {
        xml.addTag("tent");
        xml.addAttribute("tent", "id", titer->id,distance(tents.begin(), titer));
        xml.pushTag("tent",distance(tents.begin(), titer));
        
        for (vector<int>::iterator iter=titer->indices.begin();iter!=titer->indices.end();iter++) {
            xml.addTag("index");
            xml.addAttribute("index", "value", *iter,distance(titer->indices.begin(), iter));
        }
        
        for (vector<ofVec3f>::iterator iter=titer->vertices.begin();iter!=titer->vertices.end();iter++) {
            writeVertex(xml, *iter, distance(titer->vertices.begin(), iter));
            
        }
        
        
        xml.popTag();
    }
    
    if (!playground.indices.empty()) {
        xml.addTag("playground");
        xml.pushTag("playground");
        writeScreen(xml, playground, 0);
        
        for (vector<polygon>::iterator piter=polygons.begin(); piter!=polygons.end(); piter++) {
            xml.addTag("polygon");
            xml.pushTag("polygon",distance(polygons.begin(), piter));
            
            //        for (vector<int>::iterator iter=piter->indices.begin();iter!=piter->indices.end();iter++) {
            //            xml.addTag("index");
            //            xml.addAttribute("index", "value", *iter,distance(piter->indices.begin(), iter));
            //        }
            
            for (vector<ofVec3f>::iterator iter=piter->vertices.begin();iter!=piter->vertices.end();iter++) {
                writeVertex(xml, *iter, distance(piter->vertices.begin(), iter));
                
            }
            
            
            xml.popTag();
        }

        
        xml.popTag();
    }
    
     
    
    
    xml.popTag();
    
    xml.saveFile(filename);
    
    updateMatrices();
    
}

void ProjectedOverlay::log() {
    
    for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
        cout << siter->index << endl;
        cout << "indices: ";
        for (vector<int>::iterator iter=siter->indices.begin();iter!=siter->indices.end();iter++) {
            cout << *iter << " ";
        }
        cout << endl;
        //        cout << "vertices: " ;
        //
        //        for (vector<ofVec3f>::iterator viter=siter->vertices.begin();viter!=siter->vertices.end();viter++) {
        //            cout << viter->x << " " << viter->y << " " << viter->z << endl;
        //        }
    }
    
    for (vector<tent >::iterator titer=tents.begin(); titer!=tents.end(); titer++) {
        cout << "tent: " << titer->indices[0] << " " << titer->indices[1] << endl;
    }
}





void ProjectedOverlay::updateMatrices() {
    for (vector<screen>::iterator siter=screens.begin(); siter!=screens.end(); siter++) {
        ofMatrix4x4 trans = ofMatrix4x4::getTransposedOf(ofMatrix4x4::newTranslationMatrix(siter->origin));
        // double rotation - align x axis and align new y yaxis
        ofMatrix4x4 rot = ofMatrix4x4::newRotationMatrix(siter->xVec,ofVec3f(1.0f,0.0f,0.0f));
        rot.postMult(ofMatrix4x4::newRotationMatrix(siter->yVec*rot,ofVec3f(0.0f,1.0f,0.0f)));
        ofMatrix4x4 scl = ofMatrix4x4::newScaleMatrix(1.0f/scale,1.0f/scale,1.0f); 
        siter->glMat = ofMatrix4x4::getTransposedOf(trans*rot*scl);
    }
    
    for (vector<tent>::iterator iter=tents.begin(); iter!=tents.end(); iter++) {
        ofMatrix4x4 trans = ofMatrix4x4::getTransposedOf(ofMatrix4x4::newTranslationMatrix(iter->vertices[0])); // cityMesh.getVerticesPointer()[iter->first]
        
        ofMatrix4x4 rot = ofMatrix4x4::newRotationMatrix(iter->vertices[1]-iter->vertices[0],ofVec3f(1.0f,0.0f,0.0f));
        
        iter->glMat = ofMatrix4x4::getTransposedOf(trans*rot);
        
    }
}


