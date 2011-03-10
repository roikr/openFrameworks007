/*
 *  ofxSimpleSegmentator.cpp
 *  teeterTotter
 *
 *  Created by Roee Kremer on 3/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSimpleSegmentator.h"
#include "ofxXmlSettings.h"
//#include <sstream>

void ofxSimpleSegmentator::clear() {
	points.clear();
	normals.clear();
	bReady = false;
}

void ofxSimpleSegmentator::load() {
	clear();
	ofxXmlSettings xml;
	if (!xml.loadFile("segmentator.xml")) {
		return;
	}
	
	xml.pushTag("segmentator");
	
	
	roi.x = xml.getAttribute("rectangle", "x",0.0f );
	roi.y = xml.getAttribute("rectangle", "y",0.0f );
	roi.width = xml.getAttribute("rectangle", "width",0.0f );
	roi.height = xml.getAttribute("rectangle", "height",0.0f );
	
	for (int i=0; i<xml.getNumTags("vector"); i++) {
		ofxVec3f vec;
		vec.x = xml.getAttribute("vector", "x", 0.0f, i);
		vec.y = xml.getAttribute("vector", "y", 0.0f, i);
		vec.z = xml.getAttribute("vector", "z", 0.0f, i);
		addVector(vec);
	}
	xml.popTag();
}

void ofxSimpleSegmentator::save() {
	if (!getIsReady()) {
		return;
	}
	
	ofxXmlSettings xml;
	xml.addTag("segmentator");
	xml.pushTag("segmentator");
	for (int i=0; i<points.size(); i++) {
		xml.addTag("vector");
		xml.addAttribute("vector", "x", points[i].x, i);
		xml.addAttribute("vector", "y", points[i].y, i);
		xml.addAttribute("vector", "z", points[i].z, i);
	}
	
	xml.addTag("rectangle");
	xml.addAttribute("rectangle", "x", roi.x,0);
	xml.addAttribute("rectangle", "y", roi.y,0);
	xml.addAttribute("rectangle", "width", roi.width,0);
	xml.addAttribute("rectangle", "height", roi.height,0);
	
	xml.popTag();
	xml.saveFile("segmentator.xml");
}

void ofxSimpleSegmentator::setROI(ofRectangle &rect) {
	roi = rect;
}

void ofxSimpleSegmentator::addVector(ofxVec3f &vec) {
	points.push_back(vec);
	
	if (points.size()==4) {
		ofxVec3f floorNormal = -(points[1]-points[0]).cross(points[2]-points[0]);
		floorNormal.normalize();
		
		
		for (int i=0; i<points.size(); i++) {
			ofxVec3f normal = -(points[(i+1) % points.size()]-points[i]).cross(floorNormal);
			normal.normalize();
			normals.push_back(normal);
		}
		
		normals.push_back(-floorNormal);
		bReady = true;
		//if (m_normal[2] < 0) {
		//					m_normal = -m_normal;
		//				}
		
		//m_ground_plane = Plane(normal, p0);
		
		
	}
}

bool ofxSimpleSegmentator::getIsReady() {
	return bReady;
}

bool ofxSimpleSegmentator::getIsVectorInside(ofxVec3f &vec) {
		
	for (int k=0; k<normals.size(); k++) {
		if (vec.dot(normals[k]) > points[k % 4].dot(normals[k] - 0.01) ) {
			return false;
		}
	}
	
	return true;
}

bool ofxSimpleSegmentator::getIsPointInside(ofPoint &pnt) {
	return pnt.y>roi.y && pnt.y<roi.y+roi.height && pnt.x>roi.x && pnt.x<roi.x+roi.width;
}

void ofxSimpleSegmentator::getROI(ofRectangle &rect) {
	rect = this->roi;
}


void outVector(stringstream &reportStream,ofxVec3f vec) {
	reportStream << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
}

void ofxSimpleSegmentator::report(stringstream &reportStream) {
	reportStream<< "(" << roi.x << "," << roi.y << "," << roi.width << "," << roi.height << ")" << endl;

	/*
	 if (!normals.empty()) {
		 reportStream << "points: ";
		 outVector(reportStream, points[0]);
		 reportStream << " ";
		 outVector(reportStream, points[1]);
		 reportStream << " ";
		 outVector(reportStream, points[2]);
		 reportStream << " ";
		 outVector(reportStream, points[3]);
		 reportStream << endl << "normals: ";
		 outVector(reportStream, normals[0]);
	 }
	 */
}