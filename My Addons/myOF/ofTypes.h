/*
 *  ofMath.h
 *  Milgrom
 *
 *  Created by Roee Kremer on 11/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

//----------------------------------------------------------
// ofPoint
//----------------------------------------------------------

#ifndef _OF_MATH_H
#define _OF_MATH_H

class ofPoint {
public:
	virtual ~ofPoint(){}
	
    ofPoint( float _x=0.0f, float _y=0.0f, float _z=0.0f ) {
        x = _x;
        y = _y;
        z = _z;
    }
	
    ofPoint( const ofPoint & pnt){
        x = pnt.x;
        y = pnt.y;
        z = pnt.z;
    }
	
    void set(float _x, float _y, float _z = 0){
        x = _x;
        y = _y;
        z = _z;
    }
	
	
	//------ Operators:
	
  	//Negative
    ofPoint operator-() const {
        return ofPoint( -x, -y, -z );
    }
	
    //equality
    bool operator==( const ofPoint& pnt ) {
        return (x == pnt.x) && (y == pnt.y) && (z == pnt.z);
    }
	
	//inequality
    bool operator!=( const ofPoint& pnt ) {
        return (x != pnt.x) || (y != pnt.y) || (z != pnt.z);
    }
	
	//Set
	ofPoint & operator=( const ofPoint& pnt ){
		x = pnt.x;
		y = pnt.y;
		z = pnt.z;
		return *this;
	}
	
	ofPoint & operator=( const float& val ){
		x = val;
		y = val;
		z = val;
		return *this;
	}
	
	// Add
    ofPoint operator+( const ofPoint& pnt ) const {
        return ofPoint( x+pnt.x, y+pnt.y, z+pnt.z );
    }
	
    ofPoint operator+( const float& val ) const {
        return ofPoint( x+val, y+val, z+val );
    }
	
	ofPoint & operator+=( const ofPoint& pnt ) {
        x+=pnt.x;
        y+=pnt.y;
        z+=pnt.z;
        return *this;
    }
	
	ofPoint & operator+=( const float & val ) {
        x+=val;
        y+=val;
        z+=val;
        return *this;
    }
	
	// Subtract
    ofPoint operator-(const ofPoint& pnt) const {
        return ofPoint( x-pnt.x, y-pnt.y, z-pnt.z );
    }
	
    ofPoint operator-(const float& val) const {
        return ofPoint( x-val, y-val, z-val);
    }
	
    ofPoint & operator-=( const ofPoint& pnt ) {
        x -= pnt.x;
        y -= pnt.y;
        z -= pnt.z;
        return *this;
    }
	
    ofPoint & operator-=( const float & val ) {
        x -= val;
        y -= val;
        z -= val;
        return *this;
    }
	
	// Multiply
    ofPoint operator*( const ofPoint& pnt ) const {
        return ofPoint( x*pnt.x, y*pnt.y, z*pnt.z );
    }
	
    ofPoint operator*(const float& val) const {
        return ofPoint( x*val, y*val, z*val);
    }
	
    ofPoint & operator*=( const ofPoint& pnt ) {
        x*=pnt.x;
        y*=pnt.y;
        z*=pnt.z;
        return *this;
    }
	
    ofPoint & operator*=( const float & val ) {
        x*=val;
        y*=val;
        z*=val;
        return *this;
    }
	
	
	// Divide
    ofPoint operator/( const ofPoint& pnt ) const {
        return ofPoint( pnt.x!=0 ? x/pnt.x : x , pnt.y!=0 ? y/pnt.y : y, pnt.z!=0 ? z/pnt.z : z );
    }
	
    ofPoint operator/( const float &val ) const {
		if( val != 0){
			return ofPoint( x/val, y/val, z/val );
		}
        return ofPoint(x, y, z );
    }
	
    ofPoint& operator/=( const ofPoint& pnt ) {
        pnt.x!=0 ? x/=pnt.x : x;
        pnt.y!=0 ? y/=pnt.y : y;
        pnt.z!=0 ? z/=pnt.z : z;
		
        return *this;
    }
	
    ofPoint& operator/=( const float &val ) {
		if( val != 0 ){
			x /= val;
			y /= val;
			z /= val;
		}
		
		return *this;
    }
	
	// union allows us to access the coordinates through
    // both an array 'v' and 'x', 'y', 'z' member varibles
    union  {
        struct {
            float x;
            float y;
            float z;
        };
        float v[3];
    };
	
};

//----------------------------------------------------------
// ofRectangle
//----------------------------------------------------------

class ofRectangle {
public:
	ofRectangle(){ x = y = width = height = 0; };
	ofRectangle(float _x, float _y, float _w, float _h){
		x = _x;
		y = _y;
		width = _w;
		height = _h;
	}
	virtual ~ofRectangle(){}
	
	float x;
	float y;
	float width;
	float height;
	
};


#endif
