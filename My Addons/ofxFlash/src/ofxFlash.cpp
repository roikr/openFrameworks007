//
//  ofxFlash.cpp
//  flashExample
//
//  Created by Roee Kremer on 5/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxFlash.h"

#include "ofxXmlSettings.h"

#define PIXEL_SCALE 20.0
#define MAX_TEXTURE_SIZE 1024


void ofxBitmapItem::load(float u,float v) {
#ifndef TARGET_OPENGLES
    //        cout << iter->image.loadImage(iter->sourceExternalFilepath);
    cout << href << ": " << image.loadImage("LIBRARY/"+href) << endl;
    
    
#else
    ofFile file = ofFile(ofToDataPath("LIBRARY/"+href));
    if (file.exists()) {
        image.setUseTexture(false);
        cout << href << ": " << image.loadImage("LIBRARY/"+href) << endl;
        //            iter->bUseBig =  (iter->image.getWidth() > MAX_TEXTURE_SIZE || iter->image.getHeight() > MAX_TEXTURE_SIZE);
        //            if (iter->bUseBig) {
        //                iter->bigImage.loadImage(iter->image, MAX_TEXTURE_SIZE);
        //            } else {
        image.setUseTexture(true);
        image.reloadTexture();
        image.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST); // roikr: this was the trick to boost the fps as alternative to linear filtering...
        //            }
        
        
    } else {
        cout << "no image for: " << href << endl;
            file = ofFile(file.getEnclosingDirectory()+file.getBaseName()+".pvr");
            if (file.exists()) {
                cout << "load pvr for: " << name << endl;
                texture.load(file.getAbsolutePath());
                this->u = u;
                this->v = v;
                this->width = u*texture._width;
                this->height = v*texture._height;
                
            } else {
                cout << "no pvr for: " << name << " - expect problems" << endl;
            }
    }
    //        ofFile file = ofFile(iter->sourceExternalFilepath);
    
    
#endif
    
    
}

void ofxBitmapItem::draw() {
    
    if (image.bAllocated()) {
        image.draw(0,0);
    }
    
#ifdef TARGET_OPENGLES
    if (!image.bAllocated()) {
        texture.draw(u,v);
    }
#endif
     //                    if (iter->bitmapItem->bUseBig) {
    //                        iter->bitmapItem->bigImage.draw();
    //                    } else 
}

void ofxBitmapItem::bind() {
    if (image.bAllocated()) {
        image.bind();
    }
    
#ifdef TARGET_OPENGLES
    if (!image.bAllocated()) {
        glBindTexture(GL_TEXTURE_2D, texture._name);
        glEnable(GL_TEXTURE_2D);
    }
#endif
}

void ofxBitmapItem::unbind() {
    if (image.bAllocated()) {
        image.unbind();
    }
    
#ifdef TARGET_OPENGLES
    if (!image.bAllocated()) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
#endif
}

void ofxBitmapItem::release() {
    image.clear();
#ifdef TARGET_OPENGLES
    texture.release();
#endif 
}



int ofxBitmapItem::getWidth() {
#ifndef TARGET_OPENGLES
    return image.getWidth();
#else
    return image.bAllocated() ? image.getWidth() : width;
#endif
}

int ofxBitmapItem::getHeight() {
#ifndef TARGET_OPENGLES
    return image.getHeight();
#else
    return image.bAllocated() ? image.getHeight() : height;
#endif
}

void ofxDocument::setup(string name) {
   
    
    ofxXmlSettings xml;
    bitmapItems.clear();
    
    if (xml.loadFile(name)) {
        xml.pushTag("DOMDocument");
        xml.pushTag("media");
        for (int i=0; i<xml.getNumTags("DOMBitmapItem"); i++) {
            
           
//            item.sourceExternalFilepath = xml.getAttribute("DOMBitmapItem", "sourceExternalFilepath", "",i);
            
// roikr: had to comment out frameRight and frameBotton because I got wrongs values from flash 
// now I take image width and height from the image file - need to take care of ofxiTexture and transformation points            
//            item.frameRight = xml.getAttribute("DOMBitmapItem", "frameRight", 0,i);
//            item.frameBottom = xml.getAttribute("DOMBitmapItem", "frameBottom", 0,i);
//            item.width = item.frameRight/PIXEL_SCALE;
//            item.height = item.frameBottom/PIXEL_SCALE;
//          
//            cout << item.name << "\t" << item.frameRight/PIXEL_SCALE << "x" << item.frameBottom/PIXEL_SCALE << endl;
            
            bitmapItems.push_back(ofxBitmapItem(xml.getAttribute("DOMBitmapItem", "name", "",i),xml.getAttribute("DOMBitmapItem", "href", "",i)));
            
        }
        xml.popTag();
        
        xml.pushTag("symbols");
        for (int i=0; i<xml.getNumTags("Include"); i++) {
            ofxSymbolItem item;
            item.href = xml.getAttribute("Include", "href", "",i);
            symbolItems.push_back(item);
        }
        
        
        
        xml.popTag();
        
        xml.popTag();
        
    }
    
    for (vector<ofxSymbolItem>::iterator iter = symbolItems.begin(); iter!=symbolItems.end(); iter++) {
        iter->setup(this);
    }
    
    
        
    
}

ofxSymbolItem* ofxDocument::getSymbolItem(string name) {
    for (vector<ofxSymbolItem>::iterator iter = symbolItems.begin();iter!=symbolItems.end();iter++) {
        if (name == ofFile(iter->href).getBaseName()) {
            return &(*iter);
        }
    }
    cout << "could not find ofxSymbolItem: " << name << " - expect a crash" << endl;
    return  NULL;
}

ofxBitmapItem* ofxDocument::getBitmapItem(string name) {
    for (vector<ofxBitmapItem>::iterator iter = bitmapItems.begin();iter!=bitmapItems.end();iter++) {
        if (name == iter->name) {
            return &(*iter);
        }
    }
    cout << "could not find bitmapItem: " << name << " - expect a crash" << endl;
    
    return NULL;
}


void ofxDocument::load() {

#ifdef TARGET_OPENGLES
    ofxXmlSettings xml;
    map<string, pair<float,float> > textures;
    if (xml.loadFile("textures.xml")) {
        xml.pushTag("textures");
        for (int i=0;i<xml.getNumTags("texture");i++) {
            textures[xml.getAttribute("texture", "filename", "",i)] = make_pair(xml.getAttribute("texture","width",1.0,i),xml.getAttribute("texture","height",1.0,i));
        }
    }

    for (vector<ofxBitmapItem>::iterator iter=bitmapItems.begin(); iter!=bitmapItems.end(); iter++) {
        map<string, pair<float,float> >::iterator titer = textures.find(iter->name);
        if (titer!=textures.end()) {
            iter->load(titer->second.first,titer->second.second);
        } else {
            iter->load();
        }
    }
#else
    for (vector<ofxBitmapItem>::iterator iter=bitmapItems.begin(); iter!=bitmapItems.end(); iter++) {
        iter->load();
        
    }
#endif
}

void ofxDocument::release() {
    for (vector<ofxBitmapItem>::iterator iter=bitmapItems.begin(); iter!=bitmapItems.end(); iter++) {
        iter->release();
    }
    
}


vector<ofVec2f> parseVec(string str) {
    vector<ofVec2f> vec;
    vector<string> svec=ofSplitString(str, " ");
    for (vector<string>::iterator iter=svec.begin(); iter!=svec.end(); iter+=2) {
        vec.push_back(ofVec2f(ofToFloat(*iter),ofToFloat(*(iter+1))));
    }
    return vec;
}



//struct curvePath {
//    ofVec2f p0;
//    vector<vector<ofVec2f> > segments;
//    
//};
//
//struct linePath {
//    ofVec2f p0;
//    vector<ofVec2f> segments;
//    bool bClosed;
//};
//
//struct ofxBitmapFill {
//    ofxBitmapItem *bitmapItem;
//    ofMatrix4x4 mat;
//    string bitmapPath;
//};




void dumpShape(ofxShape s) {
    cout << "curvePath: " << s.curve.size() << endl;
    cout << "line: " << s.line.size() << endl;
    cout << "solidColorStroke: " << s.solidColorStroke.size() << endl;
    cout << "solidColorFill: " << s.solidColorFill.size() << endl;
    cout << "bitmapFill: " << s.bitmapFill.size() << endl;
//    for (vector<curvePath>::iterator citer=s.curve.begin(); citer!=s.curve.end(); citer++) {
//        <#statements#>
//    }
}

ofxShape parseShape(ofxXmlSettings &xml) {
    
    ofxShape s;
    
    xml.pushTag("paths");
    
    string str = xml.getAttribute("Path", "data", "");
    
    size_t found = str.find('L');
    if (found!=string::npos) {
        linePath p;
        p.p0 = parseVec(str.substr(1,found)).front(); // eliminate M (for moveTo)
        //ln.p1 = parseVec(str.substr(found+1)).front();
        
        p.bClosed = str.at(str.length()-1) == 'z';
        
        
        
        if (p.bClosed) {
            str = str.substr(found+1,str.npos-1); // eliminate z for closed
        } else {
            str = str.substr(found+1);
        }
        vector<string> svec = ofSplitString(str, "L");
//        cout << "numSegments: " << svec.size() << ", closed: " << p.bClosed<< endl;
        
        for (vector<string>::iterator iter=svec.begin(); iter!=svec.end(); iter++) {
            
            p.segments.push_back(parseVec(*iter).front());  
//            cout << p.segments.back().x << "," << p.segments.back().y << "\t";
        }
//        cout << endl;
        
        
        s.line.push_back(p);
        
//        cout << "rectangle: " << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << " " << endl;
        
    } else {
        found = str.find('C');
        if (found!=string::npos) {
            curvePath p;
            p.p0 = parseVec(str.substr(1,found)).front(); // eliminate M
            
            vector<string> svec = ofSplitString(str.substr(found+1), "C");
            //                        cout << "numCurves: " << svec.size() << endl;
            
            for (vector<string>::iterator iter=svec.begin(); iter!=svec.end(); iter++) {
                vector<ofVec2f> bezier = parseVec(*iter);
                
                //                            cout << bezier.size() << "\t";
                p.segments.push_back(bezier);
            }
            
            s.curve.push_back(p);
            
            
        }
    }
    
    xml.pushTag("Path");
    
    if (xml.tagExists("stroke")) {
        xml.pushTag("stroke");
        xml.pushTag("SolidStroke");
        xml.pushTag("fill");
        
        if (xml.tagExists("SolidColor")) {
            s.solidColorStroke.push_back(ofHexToInt(xml.getAttribute("SolidColor", "color", "").substr(1)));
        }
        xml.popTag();
        xml.popTag();
        xml.popTag();
    }
    
    if (xml.tagExists("fill")) {
        xml.pushTag("fill");
        
        if (xml.tagExists("SolidColor")) {
            s.solidColorFill.push_back(ofHexToInt(xml.getAttribute("SolidColor", "color", "").substr(1)));
        } 
        
        if (xml.tagExists("BitmapFill")) {
            ofxBitmapFill bf;
            bf.bitmapPath = xml.getAttribute("BitmapFill", "bitmapPath", "");
            xml.pushTag("BitmapFill");
            
            if (xml.tagExists("matrix")) {
                xml.pushTag("matrix");
                
                float a = xml.getAttribute("Matrix", "a", 1.0);
                float b = xml.getAttribute("Matrix", "b", 0.0);
                float c = xml.getAttribute("Matrix", "c", 0.0);
                float d = xml.getAttribute("Matrix", "d", 1.0);
                float tx = xml.getAttribute("Matrix", "tx", 0.0);
                float ty = xml.getAttribute("Matrix", "ty", 0.0);
                
                //bi.mat = ofMatrix4x4(a, c, 0, tx, b, d, 0, ty, 0, 0, 1, 0, 0, 0, 0, 1);
                //bi.mat = ofMatrix4x4(sqrt(a*a+b*b)/20, 0, 0, 0, 0,sqrt(c*c+d*d)/20, 0, 0, 0, 0, 1, 0, tx, ty, 0, 1);
                
                //                        bi.width = (float)doc.items[bi.path].frameRight/PIXEL_SCALE;
                //                        bi.height = (float)doc.items[bi.path].frameBottom/PIXEL_SCALE;
                //                        bi.href = doc.items[bi.path].href;
                
                
                //                        cout << bi.path << "\t" << bi.width << "x" << bi.height << endl;
                
                xml.popTag();
                
                bf.mat.scale(sqrt(a*a+b*b)/PIXEL_SCALE, sqrt(c*c+d*d)/PIXEL_SCALE, 1.0);
                bf.mat.rotate(atan2( b, a )*180/PI, 0, 0, 1.0);
                bf.mat.translate(ofVec2f(tx,ty));
            }
            
            
//            cout << bf.mat << endl;
            xml.popTag();
            
                        
            
            
            ofVec2f tl = s.line.front().segments[0];
            ofVec2f br = s.line.front().segments[2];
            
            bf.rect = ofRectangle(tl.x, tl.y, br.x-tl.x, br.y-tl.y); // roikr: what did I forget ? inside rect ?
            
            
            s.bitmapFill.push_back(bf);
        }
        
        xml.popTag();
    }
    
    xml.popTag(); 
    xml.popTag();
    
    return s;
}
    
    

ofxSymbolInstance parseInstance(ofxXmlSettings &xml) {
    ofxSymbolInstance i;
    
    
    
    
    if (xml.tagExists("matrix")) {
        xml.pushTag("matrix");
        ofVec2f translation = ofVec2f(xml.getAttribute("Matrix", "tx", 0.0),xml.getAttribute("Matrix", "ty", 0.0));
        float scale = xml.getAttribute("Matrix", "a", 1.0);
        float rotation = xml.getAttribute("Matrix", "c", 0.0);
        xml.popTag();
        
        i.mat.scale(scale, scale, 1.0);
        i.mat.translate(translation);
    } 
    
    if (xml.tagExists("transformationPoint")) {
        xml.pushTag("transformationPoint");
        i.transformationPoint.push_back(ofVec2f(xml.getAttribute("Point", "x", 0.0),xml.getAttribute("Point", "y", 0.0)));
        xml.popTag();
    } 
    
    i.alphaMultiplier = 1.0;
    if (xml.tagExists("color")) {
        xml.pushTag("color");
        i.alphaMultiplier = xml.getAttribute("Color", "alphaMultiplier", 1.0);
        xml.popTag();
    } 
    
    return i;
}


void ofxSymbolItem::setup(ofxDocument *doc) {
    
    ofxXmlSettings xml;
    this->doc = doc;
    
    if (xml.loadFile("LIBRARY/"+href)) {
        xml.pushTag("DOMSymbolItem");
        xml.pushTag("timeline");
        xml.pushTag("DOMTimeline");
        xml.pushTag("layers");
        for (int j=0; j<xml.getNumTags("DOMLayer");j++ ) {
            layer l;
            l.name = xml.getAttribute("DOMLayer", "name", "",j);
            xml.pushTag("DOMLayer",j);
            xml.pushTag("frames");
            xml.pushTag("DOMFrame");
            xml.pushTag("elements");
            for (int i=0; i<xml.getNumTags("DOMBitmapInstance"); i++) {
                string libraryItemName = xml.getAttribute("DOMBitmapInstance", "libraryItemName", "",i);
                xml.pushTag("DOMBitmapInstance",i);
                ofxSymbolInstance si = parseInstance(xml);
                si.bitmapItem = doc->getBitmapItem(libraryItemName);
//                if (bi.transformationPoint.empty()) {
//                    bi.transformationPoint.push_back(ofVec2f(bi.bitmapItem->width/2,bi.bitmapItem->height/2));
//                }
                xml.popTag();
                
                si.type = BITMAP_INSTANCE;
                l.instances.push_back(si);
            }
            
            for (int i=0; i<xml.getNumTags("DOMSymbolInstance"); i++) {
                xml.pushTag("DOMSymbolInstance",i);
                ofxSymbolInstance si = parseInstance(xml);
                xml.popTag();
//                si.itemID = doc->itemsMap[];
                si.name = xml.getAttribute("DOMSymbolInstance", "name", "",i);
                si.type = SYMBOL_INSTANCE;
                si.symbolItem = doc->getSymbolItem(xml.getAttribute("DOMSymbolInstance", "libraryItemName", "",i));
                l.instances.push_back(si);
            }

            
            
            for (int i=0; i<xml.getNumTags("DOMShape"); i++) {

                xml.pushTag("DOMShape",i);
                ofxSymbolInstance si;
                si.shapeIndex = shapes.size();
                shapes.push_back(parseShape(xml));
               
                if (!shapes.back().bitmapFill.empty()) {
                    ofxBitmapFill &bf = shapes.back().bitmapFill.front();
                    bf.bitmapItem = doc->getBitmapItem(bf.bitmapPath);
                }
                xml.popTag();
                si.type = SHAPE;
                l.instances.push_back(si);
            }
           
            for (int i=0; i<xml.getNumTags("DOMTLFText"); i++) {
                tlfText tlf;
                
                tlf.width = xml.getAttribute("DOMTLFText", "right", 0,i)/PIXEL_SCALE;
                tlf.height = xml.getAttribute("DOMTLFText", "bottom", 0,i)/PIXEL_SCALE;
                                
                xml.pushTag("DOMTLFText",i);
                
                xml.pushTag("matrix");
                float a = xml.getAttribute("Matrix", "a", 1.0);
                float b = xml.getAttribute("Matrix", "b", 0.0);
                float c = xml.getAttribute("Matrix", "c", 0.0);
                float d = xml.getAttribute("Matrix", "d", 1.0);
                float tx = xml.getAttribute("Matrix", "tx", 0.0);
                float ty = xml.getAttribute("Matrix", "ty", 0.0);
                
                //bi.mat = ofMatrix4x4(a, c, 0, tx, b, d, 0, ty, 0, 0, 1, 0, 0, 0, 0, 1);
                //bi.mat = ofMatrix4x4(sqrt(a*a+b*b)/20, 0, 0, 0, 0,sqrt(c*c+d*d)/20, 0, 0, 0, 0, 1, 0, tx, ty, 0, 1);
                
//                bi.width = (float)doc.items[bi.path].frameRight/PIXEL_SCALE;
//                bi.height = (float)doc.items[bi.path].frameBottom/PIXEL_SCALE;
//                bi.href = doc.items[bi.path].href;
                                    
                tlf.translation = ofVec2f(tx,ty);
                tlf.sx = sqrt(a*a+b*b)/PIXEL_SCALE;
                tlf.sy = sqrt(c*c+d*d)/PIXEL_SCALE;
                tlf.r = atan2( b, a );
                xml.popTag();
                
                xml.pushTag("markup");
                xml.pushTag("tlfTextObject");
                xml.pushTag("TextFlow");
                
                for (int j=0; j<xml.getNumTags("p"); j++) {
                    xml.pushTag("p",j);
                    for (int k=0; k<xml.getNumTags("span"); k++) {
                        string l = xml.getValue("span", "",k);
                        if (!l.empty()) {
                            span s;
                            s.text = l;
                            s.fontSize = xml.getAttribute("span", "fontSize", 12,k);
                            s.color = ofHexToInt(xml.getAttribute("span", "color", "",k).substr(1)); // eliminate the hex #
                            tlf.spans.push_back(s);
                            tlf.text.append(l+"\n");
                        }
                        
                        
                        
                    }
                    
                    xml.popTag();
                }
                    
                
                xml.popTag(); // TextFlow
                xml.popTag();
                xml.popTag();
                
                
                xml.popTag(); // DOMTLFText
                
//                cout << tlf.text << endl;
                l.texts.push_back(tlf);
            }

            
            xml.popTag();
            xml.popTag();
            xml.popTag();
            xml.popTag();
            layers.push_back(l);
        }
        
        
    
        xml.popTag();
        xml.popTag();
        xml.popTag();
        xml.popTag();
    }
    
}

// propogate alpha multiplier
ofxSymbolInstance ofxSymbolItem::createInstance(string name,ofMatrix4x4 mat,float alphaMultiplier ,ofVec2f transformationPoint) {
//    cout << this->href << "\t" << name << endl;
//    cout << mat << endl;
    ofxSymbolInstance newInstance;
    newInstance.name = name;
    newInstance.symbolItem = this;
    newInstance.mat = mat;
    newInstance.type = SYMBOL_INSTANCE;
    newInstance.alphaMultiplier = alphaMultiplier;
    newInstance.transformationPoint.push_back(transformationPoint);
    
    for (vector<layer>::iterator liter = layers.begin();liter!=layers.end();liter++) {
        layer l;
        l.name = liter->name;
        for (vector<ofxSymbolInstance>::iterator iter=liter->instances.begin();iter!=liter->instances.end();iter++) {
            switch (iter->type) {
                case BITMAP_INSTANCE: {
                    ofxSymbolInstance i;
                    i.type = iter->type;
                    i.mat = iter->mat;
                    i.transformationPoint = iter->transformationPoint;
                    i.alphaMultiplier = 1.0;
                    i.bitmapItem = iter->bitmapItem;
                    l.instances.push_back(i);
                    
                }   break;
                    
                case SHAPE: {
                    ofxSymbolInstance si;
                    si.type = iter->type;
                    si.mat = iter->mat;
                    si.transformationPoint = iter->transformationPoint;
                    si.alphaMultiplier = 1.0;
                    si.shapeIndex = iter->shapeIndex;
//                    dumpShape(si.shape);
                    l.instances.push_back(si);
                    
                }   break;
                    
                case SYMBOL_INSTANCE:
                    l.instances.push_back(iter->symbolItem->createInstance(iter->name, iter->mat,iter->alphaMultiplier,iter->transformationPoint.front()));
                    break;
                
                default:
                    break;
            }
            
        }
        newInstance.layers.push_back(l);
    }
    
    
    return newInstance;
}

/*
void ofxSymbolInstance::update() {
    
    alpha*=alphaMultiplier;
    
    switch (type) {
        case BITMAP_INSTANCE:
        case SHAPE:
            
            break;
        case SYMBOL_INSTANCE:
            for (vector<layer>::iterator liter = layers.begin();liter!=layers.end();liter++) {
                for (vector<ofxSymbolInstance>::iterator iter=liter->instances.begin();iter!=liter->instances.end();iter++) {
                    iter->alpha=alpha;
                    iter->update();
                    
                }
            }
           
            break;
        default:
            break;
    }
}
 */

/*
ofRectangle ofxSymbolItem::getScreenRect(ofRectangle& rect) {
    cout << ofGetWidth() << "\t" << ofGetHeight() << "\t" << doc->offset << "\t" << doc->zoom << "\t";
    ofVec2f p = (ofVec2f(rect.x,rect.y)+doc->offset)*doc->zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
    ofRectangle newRect(p.x,p.y,rect.width*doc->zoom , rect.height*doc->zoom);
    cout << newRect.x << "\t" << newRect.y << "\t" << newRect.width << "\t" << newRect.height << endl;
    return newRect;
}
*/



void ofxSymbolInstance::drawLayer(layer *ly,float alpha) {
    if (!ly) {
        return;
    }
    
    ofPushMatrix();
    glMultMatrixf(mat.getPtr());
    for (vector<ofxSymbolInstance>::iterator iter=ly->instances.begin(); iter!=ly->instances.end(); iter++) {
        if (iter->bVisible) {
            switch (iter->type) {
                case BITMAP_INSTANCE: {
                    ofPushMatrix();
                    glMultMatrixf(iter->mat.getPtr());
                    ofSetColor(255, 255, 255,alpha*alphaMultiplier*255.0);
                    iter->bitmapItem->draw();
                    ofSetColor(255, 255, 255,255);
                    ofPopMatrix();
                } break;
                    
                    
                case SHAPE: {
                    
                    ofxShape &shape = symbolItem->shapes[iter->shapeIndex]; 
                    if (!shape.curve.empty()) {
                        vector<curvePath>::iterator citer=shape.curve.begin(); 
                        ofBeginShape();
                        ofVertex(citer->p0.x,citer->p0.y);
                        for (vector<vector<ofVec2f> >::iterator iter=citer->segments.begin(); iter!=citer->segments.end(); iter++) {
                            
                            ofBezierVertex(iter->at(0).x,iter->at(0).y,iter->at(1).x,iter->at(1).y,iter->at(2).x,iter->at(2).y);
                        }
                        ofEndShape();
                    }
                    
                    
                    
                    if (!shape.line.empty()) {
                        //linePath l = shape.line.front();
                        vector<linePath>::iterator liter = shape.line.begin();
                        
                        if (liter->bClosed) {
                            
                            if (!shape.solidColorFill.empty()) {
                                
                                ofPushStyle();
                                ofSetHexColor(shape.solidColorFill.front());
                                
                                ofFill();
                                ofBeginShape();
                                ofVertex(liter->p0.x,liter->p0.y);
                                
                                for (vector<ofVec2f>::iterator iter=liter->segments.begin(); iter!=liter->segments.end()-1; iter++) { // roikr: we don't need the end
                                    ofVertex(iter->x,iter->y);
                                }
                                
                                ofEndShape();
                                ofPopStyle();
                                
                            };
                            
                            if (!shape.solidColorStroke.empty()) {
                                
                                ofPushStyle();
                                ofSetHexColor(shape.solidColorStroke.front());
                                
                                ofNoFill();
                                ofBeginShape();
                                ofVertex(liter->p0.x,liter->p0.y);
                                
                                for (vector<ofVec2f>::iterator iter=liter->segments.begin(); iter!=liter->segments.end(); iter++) {
                                    ofVertex(iter->x,iter->y);
                                }
                                
                                ofEndShape();
                                ofPopStyle();
                                
                            };
                            
                            if (!shape.bitmapFill.empty()) {
                                
                                ofxBitmapFill &bitmapFill = shape.bitmapFill.front();
                                
                                
//                                
//                                glEnable(GL_SCISSOR_TEST);
//                                ofRectangle &rect = bitmapFill.rect;
//                                
//#ifndef TARGET_OPENGLES                   
//                                ofVec2f p = (ofVec2f(rect.x,rect.y+rect.height)+doc->offset)*doc->zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
//                                
//                                glScissor(p.x, ofGetHeight()-p.y, rect.width*doc->zoom , rect.height*doc->zoom);
//                                
//#else                    
//                                ofVec2f q = (ofVec2f(rect.x+rect.width,rect.y+rect.height)+offset)*zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
//                                ofVec2f p = ofVec2f(ofGetHeight(),ofGetWidth())-ofVec2f(q.y,q.x);
//                                
//                                glScissor(p.x, p.y, rect.height*zoom , rect.width*zoom);
//#endif
                                
                                ofPushMatrix();
                                glMultMatrixf(bitmapFill.mat.getPtr());
                                bitmapFill.bitmapItem->draw();
                                ofPopMatrix();
                                
//                                glDisable(GL_SCISSOR_TEST);
            
                            }
                        } else {
                            
                            if (!shape.solidColorStroke.empty()) {
                                ofPushStyle();
                                ofSetHexColor(shape.solidColorStroke.front());
                                ofLine(liter->p0, liter->segments.front());
                                for (vector<ofVec2f>::iterator iter=liter->segments.begin(); iter!=liter->segments.end()-1; iter++) {
                                    ofLine(*iter, *(iter+1));
                                }
                                ofPopStyle();
                            }
                        }
                        
                    }
                }   break;
                    
            
            
            
                
                case SYMBOL_INSTANCE: 
                    iter->draw(alpha*alphaMultiplier);
                    break;
                    
                default:
                    break;
                    
            }

            
        }
    }
    
    ofPopMatrix();
     
/*    
        
        
    
    ofPushStyle();
    for (vector<tlfText>::iterator titer=ly.texts.begin();titer!=ly.texts.end();titer++) {
        
        float v = 0;
        
        for (vector<span>::iterator iter=titer->spans.begin(); iter!=titer->spans.end(); iter++) {
            ofPushMatrix();
//                ofTranslate(titer->translation+ofVec2f(0,v+iter->fontSize));
            ofTranslate(titer->translation+ofVec2f(0,v+(float)font.stringHeight(iter->text)*(float)iter->fontSize/(float)font.getSize()));
            float scale = (float)iter->fontSize/(float)font.getSize();
            ofScale(scale, scale);
            ofSetHexColor(iter->color);
            font.drawString(iter->text, 0, 0);
            ofPopMatrix();
            v+= (float)iter->fontSize*lineHeight;
        }
                        
    }
    ofPopStyle();
*/
    
    
}

void ofxSymbolInstance::draw(float alpha) {
    for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
        drawLayer(&(*riter),alpha);
    }
}


ofRectangle ofxSymbolInstance::getBoundingBox() {
//    cout << "bounding box" << endl;
    ofRectangle rect(0,0,0,0);
    
    switch (type) {
        case BITMAP_INSTANCE: {
            ofVec2f p0 = mat.preMult(ofVec3f(0,0,0));
            ofVec2f p1 = mat.preMult(ofVec3f(bitmapItem->getWidth(),bitmapItem->getHeight()));
            rect = ofRectangle(p0.x,p0.y,p1.x-p0.x,p1.y-p0.y);
        } break;
            
        case SYMBOL_INSTANCE: {
            
            for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
                for (vector<ofxSymbolInstance>::iterator iter=riter->instances.begin(); iter!=riter->instances.end(); iter++) {
                    
                    ofRectangle iterect = iter->getBoundingBox();
                    if (iterect.width!=0 || iterect.height!=0) {
                    
                        ofVec2f p0(rect.x,rect.y);
                        ofVec2f p1(p0.x+rect.width,p0.y+rect.height);
                        
                        ofVec2f q0(iterect.x,iterect.y);
                        ofVec2f q1(q0.x+iterect.width,q0.y+iterect.height);
                        
                        ofVec2f pos(MIN(p0.x,q0.x),MIN(p0.y,q0.y));
                        rect = ofRectangle(pos.x,pos.y,MAX(p1.x,q1.x)-pos.x,MAX(p1.y,q1.y)-pos.y);
                            
                       
                            //                    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
                            
                    }
                   
                }
            }
        } break;
            
        default:
            break;
    } 
    
        
    return rect;
}

vector<ofxSymbolInstance> ofxSymbolInstance::hitLayer(layer *lyr,ofVec2f pos)  {
    vector<ofxSymbolInstance> instances;
    
    pos = mat.getInverse().preMult(ofVec3f(pos));
    
    for (vector<ofxSymbolInstance>::iterator iter=lyr->instances.begin(); iter!=lyr->instances.end(); iter++) {
        
        //            cout << wpos << "\t" << endl;
        
        switch (iter->type) {
            case BITMAP_INSTANCE: {
                ofMatrix4x4 imat = iter->mat.getInverse();
                ofVec2f wpos = imat.preMult(ofVec3f(pos));
                
                if (ofRectangle(0,0,iter->bitmapItem->getWidth(),iter->bitmapItem->getHeight()).inside(wpos)) {
                    
                    instances.push_back(*iter);
                }
            }   break;
            case SYMBOL_INSTANCE: {
                vector<ofxSymbolInstance> tempInstances = iter->hitTest(pos);
                if (!tempInstances.empty()) {
                    instances.push_back(*iter);
                    instances.insert(instances.end(), tempInstances.begin(), tempInstances.end());
                }
                
            }   break;
                
            default:
                break;
                
        }
        //            cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
        
    }
    
    return instances;
}


vector<ofxSymbolInstance> ofxSymbolInstance::hitTest(ofVec2f pos) {
//    cout << "testing" << endl;
    
    vector<ofxSymbolInstance> instances;
    
    for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
        vector<ofxSymbolInstance> ins = hitLayer(&*riter, pos);
        instances.insert(instances.end(), ins.begin(), ins.end());
    }
    
    return instances;
}

ofxSymbolInstance *ofxSymbolInstance::getChild(string name) {
    for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
        for (vector<ofxSymbolInstance>::iterator iter=riter->instances.begin(); iter!=riter->instances.end(); iter++) {
            if (iter->type==SYMBOL_INSTANCE && iter->name == name) {
                return &(*iter);
            }
        }
    }
    
    cout << "could not found ofxSymbolInstance: " << name << " - expect a crash" << endl;
    return NULL;
}


bool ofxSymbolInstance::getChildMat(ofxSymbolInstance *child,ofMatrix4x4 &mat) {
    if (this==child) {
        mat = this->mat;
        return true;
    }
    
    for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
        for (vector<ofxSymbolInstance>::iterator iter=riter->instances.begin(); iter!=riter->instances.end(); iter++) {
            if (iter->getChildMat(child, mat)) {
                mat.postMult(this->mat);
                return  true;
            }
        }
    }
    
    return false;
}

layer *ofxSymbolInstance::getLayer(string name) {
    for (vector<layer>::iterator iter=layers.begin();iter!=layers.end();iter++) {
        if (iter->name == name) {
            return &(*iter);
        }
    }
    
    cout << "could not not found layer: " << name << " - expect a crash" << endl;
    return NULL;
    
}

vector<string> ofxSymbolInstance::listLayers() {
    vector<string> res;
    for (vector<layer>::iterator iter=layers.begin();iter!=layers.end();iter++) {
        res.push_back(iter->name);
    }
    return res;
}

vector<string> ofxSymbolInstance::listChilds(string name) {
    vector<string> res;
    layer *l = getLayer(name);
    if (l!=NULL) {
        for (vector<ofxSymbolInstance>::iterator iter=l->instances.begin(); iter!=l->instances.end(); iter++) {
            if (iter->type==SYMBOL_INSTANCE) {
                res.push_back(iter->name);
            }
        }
    }
    return res;
}
