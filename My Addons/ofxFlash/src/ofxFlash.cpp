//
//  ofxFlash.cpp
//  flashExample
//
//  Created by Roee Kremer on 5/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxFlash.h"

#include "ofxXmlSettings.h"


void ofxDocument::setup(string name) {
   
    
    ofxXmlSettings xml;
    bitmapItems.clear();
    
    if (xml.loadFile(name)) {
        xml.pushTag("DOMDocument");
        xml.pushTag("media");
        for (int i=0; i<xml.getNumTags("DOMBitmapItem"); i++) {
            
            bitmapItem item;
            item.name = xml.getAttribute("DOMBitmapItem", "name", "",i);
//            item.sourceExternalFilepath = xml.getAttribute("DOMBitmapItem", "sourceExternalFilepath", "",i);
            item.href = xml.getAttribute("DOMBitmapItem", "href", "",i);
            item.frameRight = xml.getAttribute("DOMBitmapItem", "frameRight", 0,i);
            item.frameBottom = xml.getAttribute("DOMBitmapItem", "frameBottom", 0,i);
            item.width = item.frameRight/PIXEL_SCALE;
            item.height = item.frameBottom/PIXEL_SCALE;
            bitmapItems.push_back(item);
//            cout << name << "\t" << frameRight/20 << "x" << frameBottom/20 << endl;
            itemsMap[item.name]=bitmapItems.size()-1;
        }
        xml.popTag();
        
        xml.pushTag("symbols");
        for (int i=0; i<xml.getNumTags("Include"); i++) {
            ofxSymbolItem item;
            item.href = xml.getAttribute("Include", "href", "",i);
            itemsMap[ofFile(item.href).getBaseName()]=symbolItems.size();
            symbolItems.push_back(item);
        }
        
        
        
        xml.popTag();
        
        xml.popTag();
        
    }
    
    for (vector<ofxSymbolItem>::iterator iter = symbolItems.begin(); iter!=symbolItems.end(); iter++) {
        iter->setup(this);
    }
    
}


void ofxDocument::load() {

    for (vector<bitmapItem>::iterator iter=bitmapItems.begin(); iter!=bitmapItems.end(); iter++) {
        
        
        
#ifndef TARGET_OPENGLES
//        cout << iter->libraryItemName << endl;
//        cout << iter->image.loadImage("LIBRARY/"+iter->libraryItemName);
//        cout << iter->image.loadImage(iter->sourceExternalFilepath);
        cout << iter->href << ": " << iter->image.loadImage("LIBRARY/"+iter->href) << endl;
        iter->image.update();
        
        //            cout << iter->href << " loaded" <<endl;
#else
        ofFile file = ofFile(ofToDataPath("LIBRARY/"+iter->href));
        if (file.exists()) {
            cout << iter->href << ": " << iter->image.loadImage("LIBRARY/"+iter->href) << endl;
        } else {
            file = ofFile(file.getEnclosingDirectory()+file.getBaseName()+".pvr");
            if (file.exists()) {
                cout << "load pvr for: " << file.getAbsolutePath();
                iter->texture.load(file.getAbsolutePath());
                iter->uWidth = iter->width/(float)iter->texture._width;
                iter->vHeight =  iter->height/(float)iter->texture._height;
            } else {
                cout << "no pvr for: " << file.getAbsolutePath();
            }
        }
 //        ofFile file = ofFile(iter->sourceExternalFilepath);
        
        
#endif
        
        
    }

/*
    for (vector<shape>::iterator siter=liter->shapes.begin(); siter!=liter->shapes.end(); siter++) {
        if (!siter->bitmapFill.empty()) {
            vector<bitmap>::iterator iter = siter->bitmapFill.begin();
            
#ifndef TARGET_OPENGLES
            iter->image.loadImage("LIBRARY/"+iter->href);
            iter->image.update();
            cout << iter->href << " loaded" <<endl;
#else
            ofFile file = ofFile("LIBRARY/"+iter->path);
            iter->texture.load(file.getEnclosingDirectory()+file.getBaseName()+".pvr");
#endif                
            
            
            
        }
        
    }
*/
    
    
}

void ofxDocument::release() {
    for (vector<bitmapItem>::iterator iter=bitmapItems.begin(); iter!=bitmapItems.end(); iter++) {
        iter->image.clear();
#ifdef TARGET_OPENGLES
        iter->texture.release();
#endif
        
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

instance parseInstance(ofxXmlSettings &xml) {
    instance i;
    if (xml.tagExists("matrix")) {
        xml.pushTag("matrix");
        i.translation = ofVec2f(xml.getAttribute("Matrix", "tx", 0.0),xml.getAttribute("Matrix", "ty", 0.0));
        i.scale = xml.getAttribute("Matrix", "a", 1.0);
        i.rotation = xml.getAttribute("Matrix", "c", 0.0);
        xml.popTag();
    } else {
        i.translation = ofVec2f(0.0,0.0);
        i.scale = 1.0;
        i.rotation = 0.0;
    }
    
    ofMatrix4x4 mat;
    mat.makeIdentityMatrix();
    mat.translate(i.translation);
    mat.scale(i.scale, i.scale, 1.0);
    i.mat = mat;
    
    if (xml.tagExists("transformationPoint")) {
        xml.pushTag("transformationPoint");
        i.transformationPoint = ofVec2f(xml.getAttribute("Point", "x", 0.0),xml.getAttribute("Point", "y", 0.0));
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
                int itemID = doc->itemsMap[xml.getAttribute("DOMBitmapInstance", "libraryItemName", "",i)];
                xml.pushTag("DOMBitmapInstance",i);
                instance bi = parseInstance(xml);
                bi.itemID = itemID;
                if (!xml.tagExists("transformationPoint")) {
                    bi.transformationPoint = ofVec2f(doc->bitmapItems[bi.itemID].width/2,doc->bitmapItems[bi.itemID].height/2);
                }
                xml.popTag();
                
                bi.type = BITMAP_INSTANCE;
                l.instances.push_back(bi);
            }
            
            for (int i=0; i<xml.getNumTags("DOMSymbolInstance"); i++) {
                xml.pushTag("DOMSymbolInstance",i);
                instance si = parseInstance(xml);
                xml.popTag();
                si.itemID = doc->itemsMap[xml.getAttribute("DOMSymbolInstance", "libraryItemName", "",i)];
                si.name = xml.getAttribute("DOMSymbolInstance", "name", "",i);
                si.type = SYMBOL_INSTANCE;
                l.instances.push_back(si);
            }

            
            
            for (int i=0; i<xml.getNumTags("DOMShape"); i++) {
                shape s;
                
                xml.pushTag("DOMShape",i);
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
//                    cout << "numSegments: " << svec.size() << ", closed: " << p.bClosed<< endl;
                    
                    for (vector<string>::iterator iter=svec.begin(); iter!=svec.end(); iter++) {
                                                
                        p.segments.push_back(parseVec(*iter).front());  
//                         cout << p.segments.back().x << "," << p.segments.back().y << "\t";
                    }
//                    cout << endl;
                    
                    
                    s.line.push_back(p);
                    
//                    cout << "rectangle: " << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << " " << endl;
                    
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
                        bitmap bi;
                        bi.itemID = doc->itemsMap[xml.getAttribute("BitmapFill", "bitmapPath", "")];
                        xml.pushTag("BitmapFill");
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
                        
                        bi.translation = ofVec2f(tx,ty);
                        bi.sx = sqrt(a*a+b*b)/PIXEL_SCALE;
                        bi.sy = sqrt(c*c+d*d)/PIXEL_SCALE;
                        bi.r = atan2( b, a );
                        xml.popTag();
                        xml.popTag();
                        
                        
                        ofVec2f tl = s.line.front().segments[0];
                        ofVec2f br = s.line.front().segments[2];
                        
                        bi.rect = ofRectangle(tl.x, tl.y, br.x-tl.x, br.y-tl.y);
                        
                        
                        s.bitmapFill.push_back(bi);
                    }
                    
                    xml.popTag();
                }
                
                xml.popTag(); 
                xml.popTag();           
                xml.popTag();
                l.shapes.push_back(s);
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

/*
ofRectangle ofxSymbolItem::getScreenRect(ofRectangle& rect) {
    cout << ofGetWidth() << "\t" << ofGetHeight() << "\t" << doc->offset << "\t" << doc->zoom << "\t";
    ofVec2f p = (ofVec2f(rect.x,rect.y)+doc->offset)*doc->zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
    ofRectangle newRect(p.x,p.y,rect.width*doc->zoom , rect.height*doc->zoom);
    cout << newRect.x << "\t" << newRect.y << "\t" << newRect.width << "\t" << newRect.height << endl;
    return newRect;
}
*/

void ofxSymbolItem::bitmapFill(bitmap &bm) {
    
//    glEnable(GL_SCISSOR_TEST);
//    ofRectangle &rect = bm.rect;
//    
//#ifndef TARGET_OPENGLES                   
//    ofVec2f p = (ofVec2f(rect.x,rect.y+rect.height)+doc->offset)*doc->zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
//    
//    glScissor(p.x, ofGetHeight()-p.y, rect.width*doc->zoom , rect.height*doc->zoom);
//    
//#else                    
//    ofVec2f q = (ofVec2f(rect.x+rect.width,rect.y+rect.height)+offset)*zoom+0.5*ofVec2f(ofGetWidth(),ofGetHeight());
//    ofVec2f p = ofVec2f(ofGetHeight(),ofGetWidth())-ofVec2f(q.y,q.x);
//    
//    glScissor(p.x, p.y, rect.height*zoom , rect.width*zoom);
//#endif
//    
//    
//    
//    
//    ofPushMatrix();
//    //                glMultMatrixf(iter->mat.getPtr());
//    ofTranslate(bm.translation);
//    ofRotate(bm.r*180/PI);
//    ofScale(bm.sx, bm.sy);
//    
//    
//#ifndef TARGET_OPENGLES
//   doc->bitmapItems[bm.itemID].image.draw(0, 0);
//#else
//    doc->bitmapItems[bm.itemID].texture.draw();
//#endif
//    
//    
//    ofPopMatrix();
//    
//    glDisable(GL_SCISSOR_TEST);
}


void ofxSymbolItem::drawLayer(instance &si,layer &ly) {
    ofPushMatrix();
    glMultMatrixf(si.mat.getPtr());
    for (vector<instance>::iterator iter=ly.instances.begin(); iter!=ly.instances.end(); iter++) {
        
//        ofTranslate(iter->translation);
//        ofScale(iter->scale, iter->scale);
//      
        
        
        switch (iter->type) {
            case BITMAP_INSTANCE: {
                ofPushMatrix();
                glMultMatrixf(iter->mat.getPtr());
                //            iter->texture.draw(iter->u,iter->v);
                bitmapItem &item = doc->bitmapItems[iter->itemID];
#ifndef TARGET_OPENGLES
                item.image.draw(0, 0);
#else
                if (item.image.bAllocated()) {
                    item.image.draw(0, 0);
                } else {
                    item.texture.draw(item.uWidth,item.vHeight);
                }
            
#endif
                
                ofPopMatrix();
            } break;
            
            case SYMBOL_INSTANCE: {
                //            iter->texture.draw(iter->u,iter->v);
                ofxSymbolItem &item = doc->symbolItems[iter->itemID];
                item.draw(*iter);
            } break;
                
        }

        
    }
    
     
    
    for (vector<shape>::iterator siter=ly.shapes.begin(); siter!=ly.shapes.end(); siter++) {
        
        
        if (!siter->curve.empty()) {
            vector<curvePath>::iterator citer=siter->curve.begin(); 
            ofBeginShape();
            ofVertex(citer->p0.x,citer->p0.y);
            for (vector<vector<ofVec2f> >::iterator iter=citer->segments.begin(); iter!=citer->segments.end(); iter++) {
                
                ofBezierVertex(iter->at(0).x,iter->at(0).y,iter->at(1).x,iter->at(1).y,iter->at(2).x,iter->at(2).y);
            }
            ofEndShape();
        }
        
        
        
        if (!siter->line.empty()) {
            //linePath l = siter->line.front();
            vector<linePath>::iterator liter = siter->line.begin();
            
            if (liter->bClosed) {

                if (!siter->solidColorFill.empty()) {
                    
                    ofPushStyle();
                    ofSetHexColor(siter->solidColorFill.front());
                    
                    ofFill();
                    ofBeginShape();
                    ofVertex(liter->p0.x,liter->p0.y);
                    
                    for (vector<ofVec2f>::iterator iter=liter->segments.begin(); iter!=liter->segments.end()-1; iter++) { // roikr: we don't need the end
                        ofVertex(iter->x,iter->y);
                    }
                    
                    ofEndShape();
                    ofPopStyle();
                    
                };
                
                if (!siter->solidColorStroke.empty()) {
                    
                    ofPushStyle();
                    ofSetHexColor(siter->solidColorStroke.front());
                    
                    ofNoFill();
                    ofBeginShape();
                    ofVertex(liter->p0.x,liter->p0.y);
                    
                    for (vector<ofVec2f>::iterator iter=liter->segments.begin(); iter!=liter->segments.end(); iter++) {
                        ofVertex(iter->x,iter->y);
                    }

                    ofEndShape();
                    ofPopStyle();
                    
                };
                
                if (!siter->bitmapFill.empty()) {
                    bitmapFill(siter->bitmapFill.front());
                    
                }
            } else {
                
                if (!siter->solidColorStroke.empty()) {
                    ofPushStyle();
                    ofSetHexColor(siter->solidColorStroke.front());
                    ofLine(liter->p0, liter->segments.front());
                    for (vector<ofVec2f>::iterator iter=liter->segments.begin(); iter!=liter->segments.end()-1; iter++) {
                        ofLine(*iter, *(iter+1));
                    }
                    ofPopStyle();
                }
            }
            
        }
   
        
    }
    
    
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
    
    ofPopMatrix();
}

void ofxSymbolItem::draw(instance &si) {
    for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
        drawLayer(si,*riter);
    }
}

/*
ofRectangle ofxSymbolItem::getBoundingBox() {
//    cout << "bounding box" << endl;
    ofRectangle rect(0,0,0,0);
    for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
        for (vector<instance>::iterator iter=riter->instances.begin(); iter!=riter->instances.end(); iter++) {
            switch (iter->type) {
                case BITMAP_INSTANCE: {
                    bitmapItem &item = doc->bitmapItems[iter->itemID];
                    ofVec2f p0 = iter->mat.preMult(ofVec3f(0,0,0));
                    ofVec2f p1 = iter->mat.preMult(ofVec3f(item.width,item.height));
                    
                    if (rect.width==0 || rect.height==0) {
                        rect = ofRectangle(p0.x,p0.y,p1.x-p0.x,p1.y-p0.y);
                    } else {
                        ofVec2f q0(rect.x,rect.y);
                        ofVec2f q1(q0.x+rect.width,q0.y+rect.height);
                        ofVec2f pos(MIN(p0.x,q0.x),MIN(p0.y,q0.y));
                        rect = ofRectangle(pos.x,pos.y,MAX(p1.x,q1.x)-pos.x,MAX(p1.y,q1.y)-pos.y);
                    }
//                    cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;
                    
                }
                    
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    return rect;
}
*/

vector<instance> ofxSymbolItem::hitTest(instance &si,ofVec2f pos) {
//    cout << "testing" << endl;
    vector<instance> items;
    
    pos = si.mat.getInverse().preMult(ofVec3f(pos));
    
    for (vector<layer>::reverse_iterator riter=layers.rbegin();riter!=layers.rend();riter++) {
        for (vector<instance>::iterator iter=riter->instances.begin(); iter!=riter->instances.end(); iter++) {
            
//            cout << wpos << "\t" << endl;
           
            switch (iter->type) {
                case BITMAP_INSTANCE: {
                    ofMatrix4x4 imat = iter->mat.getInverse();
                    ofVec2f wpos = imat.preMult(ofVec3f(pos));
                    bitmapItem &item = doc->bitmapItems[iter->itemID];
                    if (ofRectangle(0,0,item.width,item.height).inside(wpos)) {
                        
                        items.push_back(*iter);
                    }
                }   break;
                case SYMBOL_INSTANCE: {
                    vector<instance> tempItems = doc->symbolItems[iter->itemID].hitTest(*iter, pos);
                    if (!tempItems.empty()) {
                        items.push_back(*iter);
                        items.insert(items.end(), tempItems.begin(), tempItems.end());
                    }
                    
                }   break;
                    
                default:
                    break;
                    
            }
//            cout << rect.x << "\t" << rect.y << "\t" << rect.width << "\t" << rect.height << endl;

        }
    }
    
    return items;
}