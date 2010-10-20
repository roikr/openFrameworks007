/*
 *  perspectiveWarper.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 6/11/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "ofxPerspectiveWarper.h"
#include "ofxVec2f.h"

static const float DISTANCE_THRESH = 100.0f;
static const float DISTANCE_THRESH_SQ = DISTANCE_THRESH*DISTANCE_THRESH;

ofxPerspectiveWarper::ofxPerspectiveWarper()
{
	bEditing = false;
	mouse_index = -1;
	invert = false;
	
	ofAddListener(ofEvents.mouseMoved, this, &ofxPerspectiveWarper::mouseMoved);
	ofAddListener(ofEvents.mouseDragged, this, &ofxPerspectiveWarper::mouseDragged);
	
	editOffset.set(0,0);
	editScale = 1;
}

ofxPerspectiveWarper::~ofxPerspectiveWarper()
{
	cvReleaseMat( &warp_translate );
	cvReleaseMat( &warp_translate_inv );
	
	ofRemoveListener(ofEvents.mouseMoved, this, &ofxPerspectiveWarper::mouseMoved);
	ofRemoveListener(ofEvents.mouseDragged, this, &ofxPerspectiveWarper::mouseDragged);
	
	delete [] u;
	delete [] v;
}

void ofxPerspectiveWarper::initWarp( float _orig_width, float _orig_height, int nx, int ny )
{
	num_x_sections = nx;
	num_y_sections = ny;
	
	// allocate array num_y_sections+1 x num_x_sections+1
	u = new float[ (num_y_sections+1)*(num_x_sections+1)];
	v = new float[ (num_y_sections+1)*(num_x_sections+1)];
	/*u.resize( num_y_sections + 1 );
	v.resize( num_y_sections + 1 );
	for ( int i=0; i<num_y_sections + 1; i++ )
	{
		u[i].resize( num_x_sections+1 );
		v[i].resize( num_x_sections+1 );
	}*/
	
	initWarp(_orig_width, _orig_height);
	
}

void ofxPerspectiveWarper::initWarp( float _orig_width, float _orig_height )
{
	orig_width = _orig_width;
	orig_height = _orig_height;
	warp_translate = cvCreateMat( 3, 3, CV_32FC1 );
	warp_translate_inv = cvCreateMat( 3, 3, CV_32FC1 );
	dest_points[0].set( 0.0f, 0.0f );
	dest_points[1].set( orig_width, 0.0f );
	dest_points[2].set( orig_width, orig_height );
	dest_points[3].set( 0.0f, orig_height );
	
	for( int i = 0; i < 4; i++)
		points[i] = dest_points[i];
	
	warp_transform_storage_in = cvCreateMat( 1,1, CV_32FC2 );
	warp_transform_storage_out = cvCreateMat( 1,1, CV_32FC2 );
	
	// setup points array
	//resetPoints();
	
}

void ofxPerspectiveWarper::shutdownWarp()
{
	cvReleaseMat( &warp_translate );
	cvReleaseMat( &warp_translate_inv );
	cvReleaseMat( &warp_transform_storage_in );
	cvReleaseMat( &warp_transform_storage_out );
}

void ofxPerspectiveWarper::recalculateWarp( ofPoint src[4], ofPoint dst[4] )
{
	for ( int i=0; i<4; i++ )
	{
		points[i] = src[i];
		dest_points[i] = dst[i];
	}
	recalculateWarp();
}

void ofxPerspectiveWarper::recalculateWarp( float new_points[] )
{
	for ( int i=0; i<4; i++ )
	{
		points[i].x = new_points[2*i];
		points[i].y = new_points[2*i+1];
	}
	recalculateWarp();
}

void ofxPerspectiveWarper::recalculateWarp()
{
	//printf("%f %f\n", orig_width, orig_height );
	// compute matrix for perspectival warping (homography)
	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];
	cvSetZero( warp_translate );
	for (int i = 0; i < 4; i++ ) {
		cvsrc[i].x = points[i].x;
		cvsrc[i].y = points[i].y;
		cvdst[i].x = dest_points[i].x;
		cvdst[i].y = dest_points[i].y;
	}
	if ( invert )
	{
		cvWarpPerspectiveQMatrix( cvdst, cvsrc, warp_translate );  // calculate homography
		cvWarpPerspectiveQMatrix( cvsrc, cvdst, warp_translate_inv );
	}
	else
	{
		cvWarpPerspectiveQMatrix( cvsrc, cvdst, warp_translate );  // calculate homography
		cvWarpPerspectiveQMatrix( cvdst, cvsrc, warp_translate_inv );
	}
	
	
	if( num_x_sections > 0 && num_y_sections > 0 ) reCalculateUV();
	
}

void ofxPerspectiveWarper::dumpWarpMatrix()
{
	for ( int i=0; i<3; i++)
		printf("%f %f %f\n",
			   cvmGet( warp_translate, i, 0 ),
			   cvmGet( warp_translate, i, 1 ),
			   cvmGet( warp_translate, i, 2 ) );
}

ofPoint ofxPerspectiveWarper::warpPoint( const ofPoint& p )
{
	// warp p by the warp_translate matrix
	
	cvSet2D(warp_transform_storage_in, 0, 0, cvScalar( p.x, p.y ) );
	cvPerspectiveTransform( warp_transform_storage_in, warp_transform_storage_out, warp_translate );
	
	CvScalar result = cvGet2D( warp_transform_storage_out, 0, 0 );
	
	return ofPoint( result.val[0], result.val[1] );
}

ofPoint ofxPerspectiveWarper::warpPointInv( const ofPoint& p )
{
	// warp p by the warp_translate matrix
	
	cvSet2D(warp_transform_storage_in, 0, 0, cvScalar( p.x, p.y ) );
	cvPerspectiveTransform( warp_transform_storage_in, warp_transform_storage_out, warp_translate_inv );
	
	CvScalar result = cvGet2D( warp_transform_storage_out, 0, 0 );
	
	return ofPoint( result.val[0], result.val[1] );
}

void ofxPerspectiveWarper::drawEditInterface( float x, float y, float scale )
{
	editOffset.set(x,y);
	editScale = scale;
	
	ofNoFill();
	
	ofRect(x,y,orig_width*scale,orig_height*scale);
	
	for ( int i=0; i<4; i++ )
	{
		ofSetColor( 0x77FF77 );
		ofLine( 
			   x+(points[i].x*scale), y+(points[i].y*scale),
			   x+(points[(i+1)%4].x*scale), y+(points[(i+1)%4].y*scale) );
		
		if ( mouse_index == i ) ofSetColor( 0x7777ff );
		ofCircle( x+(points[i].x*scale), y+(points[i].y*scale), 10*scale );
	}
	
	ofSetColor(0xffffff);
}


void ofxPerspectiveWarper::mouseMoved(ofMouseEventArgs& event)
{
	if ( !bEditing ) return;
	
	float x = event.x;
	float y = event.y;
	
	ofxVec2f mouse_pos( (x/editScale)-editOffset.x, (y/editScale)-editOffset.y );
	
	int best_distance = (mouse_pos - points[0]).lengthSquared();
	int best_index = 0;
	for ( int i=1; i<4; i++ )
	{
		int distance = (mouse_pos - points[i]).lengthSquared();
		if ( distance < best_distance )
		{
			best_distance = distance;
			best_index = i;
		}
	}
	
	if ( best_distance < DISTANCE_THRESH_SQ )
		mouse_index = best_index;
	else
		mouse_index = -1;
	
	
}

void ofxPerspectiveWarper::mouseDragged(ofMouseEventArgs& event)
{
	if ( !bEditing ) return;
	
	float x = event.x;
	float y = event.y;
	
	int dx = x-editOffset.x;//px;
	int dy = y-editOffset.y;//py;
	//px = x;
	//py = y;
	
	if ( mouse_index != -1 )
	{
		points[mouse_index].x = dx*(1/editScale);
		points[mouse_index].y = dy*(1/editScale);
		recalculateWarp();
	}
	
	
}


void ofxPerspectiveWarper::saveToXml( string filename )
{
	ofxXmlSettings xml;
	
	xml.addTag("warper");
	xml.pushTag("warper");
	
	for ( int i=0; i<4; i++ )
	{
		int index = xml.addTag( "point" );
		xml.pushTag( "point", index );
		xml.addValue( "x", points[i].x );
		xml.addValue( "y", points[i].y );
		xml.popTag();
	}
	
	xml.saveFile(filename);
}

void ofxPerspectiveWarper::loadFromXml(string filename )
{
	ofxXmlSettings xml;
	xml.loadFile(filename);
	
	xml.pushTag("warper");
	
	int count = xml.getNumTags( "point" );
	assert( count == 0 || count == 4 );
	if( count == 0 )
	{
		resetPoints();
	}
	else
		for ( int i=0; i<4; i++ )
		{
			xml.pushTag( "point", i );
			points[i].x = xml.getValue( "x", 0.0f );
			points[i].y = xml.getValue( "y", 0.0f );
			xml.popTag();
		}
	
	xml.popTag();
	
	recalculateWarp();
}


void ofxPerspectiveWarper::resetPoints()
{
	points[0].set( 0.5f, 0.5f );
	points[1].set( 100.0f, 0.5f );
	points[2].set( 100.0f, 100.0f );
	points[3].set( 0.5f, 100.0f );
}


void ofxPerspectiveWarper::reCalculateUV()
{
	GLint px0 = 0;		// up to you to get the aspect ratio right
	GLint py0 = 0;
	GLint px1 = (GLint)orig_width;
	GLint py1 = (GLint)orig_height;
	
	// for rect mode center, let's do this:
	if (ofGetRectMode() == OF_RECTMODE_CENTER){
		px0 = (GLint)-orig_width/2;
		py0 = (GLint)-orig_height/2;
		px1 = (GLint)+orig_width/2;
		py1 = (GLint)+orig_height/2;
	}
	
	
	GLfloat x_step = (px1-px0)/(float)num_x_sections;
	GLfloat y_step = (py1-py0)/(float)num_y_sections;
	
	GLfloat curr_y = py0;
	
	for ( int i=0; i<num_y_sections+1; i++ )
	{
		GLfloat curr_x = px0;
		
		for ( int j=0; j<num_x_sections+1; j++ )
		{
			// warp
			ofPoint warped = warpPointInv( ofPoint( curr_x, curr_y ) );
			
			int p = i * (num_x_sections+1) + j;
			
			// store
			u[p] = warped.x;
			v[p] = warped.y;
			
			// step
			curr_x += x_step;
		}
		
		// and step
		curr_y += y_step;
	}
}


void ofxPerspectiveWarper::drawUV( float x, float y, float scale )
{
	glLineWidth(1);
	
	glPushMatrix();
	glTranslatef(x,y,0);
	for ( int i=0; i<num_y_sections+1; i++ )
	{
		for ( int j=0; j<num_x_sections+1; j++ )
		{
			int p = i * (num_x_sections+1) + j;
			
			//ofRect(u[p]*scale,v[p]*scale,1*scale,1*scale);
			if(j < num_x_sections)
			{
				int p1 = i * (num_x_sections+1) + (j+1);
				ofLine(u[p]*scale,v[p]*scale,u[p1]*scale,v[p1]*scale);
			}
			
			if(i < num_y_sections)
			{
				int p1 = (i+1) * (num_x_sections+1) + j;
				ofLine(u[p]*scale,v[p]*scale,u[p1]*scale,v[p1]*scale);
			}
		}
	}
	glPopMatrix();
}


void ofxPerspectiveWarper::drawTexture(ofTexture &tex,int x, int y, int w, int h)
{

	int nX = num_x_sections;
	int nY = num_y_sections;
	ofTextureData &texData = tex.texData;
	
	glEnable(texData.textureTarget);
	
	// bind the texture
	glBindTexture( texData.textureTarget, (GLuint)texData.textureID );
	
	GLfloat px0 = 0;		// up to you to get the aspect ratio right
	GLfloat py0 = 0;
	GLfloat px1 = w;
	GLfloat py1 = h;
	
	if (texData.bFlipTexture == true){
		GLint temp = (GLint)py0;
		py0 = py1;
		py1 = temp;
	}
	
	// for rect mode center, let's do this:
	if (ofGetRectMode() == OF_RECTMODE_CENTER){
		px0 = -w/2;
		py0 = -h/2;
		px1 = +w/2;
		py1 = +h/2;
	}
	
	//we translate our drawing points by our anchor point.
	//we still respect ofRectMode so if you have rect mode set to
	//OF_RECTMODE_CENTER your anchor will be relative to that.
	GLfloat anchorX = 0;
	GLfloat anchorY = 0;
	
//	if(bAnchorIsPct){
//		anchorX = anchor.x * w;
//		anchorY = anchor.y * h;
//	}else{
//		anchorX = anchor.x;
//		anchorY = anchor.y;
//	}
	
	px0 -= anchorX;
	py0 -= anchorY;
	px1 -= anchorX;
	py1 -= anchorY;
	
	
	// -------------------------------------------------
	// complete hack to remove border artifacts.
	// slightly, slightly alters an image, scaling...
	// to remove the border.
	// we need a better solution for this, but
	// to constantly add a 2 pixel border on all uploaded images
	// is insane..
	
	GLfloat offsetw = 0;
	GLfloat offseth = 0;
	

#ifdef TARGET_OPENGLES	
	
	if (texData.textureTarget == GL_TEXTURE_2D) {  //  && bTexHackEnabled
		offsetw = 1.0f / (texData.tex_w);
		offseth = 1.0f / (texData.tex_h);
	}
#endif
	
	
	
	GLfloat tx0 = 0+offsetw;
	GLfloat ty0 = 0+offseth;
	GLfloat tx1 = texData.tex_t - offsetw;
	GLfloat ty1 = texData.tex_u - offseth;
	
	glPushMatrix(); 
	
	glTranslatef(x,y,0.0f);
	
	// keep the x,y solid, vary the u,v
	float x_step = (px1-px0)/(float)nX;
	float y_step = (py1-py0)/(float)nY;
	
	float curr_y = py0;
	float next_y = py0+y_step;
	
	glBegin(GL_QUADS);
	
	for ( int i=0; i<nY; i++ )
	{
		float curr_x = px0;
		float next_x = px0+x_step;
		
		int curr_i = texData.bFlipTexture?(nY-i):i;
		int next_i = texData.bFlipTexture?(nY-1-i):i+1;
		
		
		for ( int j=0; j<nX; j++ )
		{
			
			int p  = curr_i * (nX+1) + j;
			int p1 = curr_i * (nX+1) + (j+1);
			int p2 = next_i * (nX+1) + (j+1);
			int p3 = next_i * (nX+1) + j;
			
			// draw this qua
			glTexCoord2f( curr_x, h-curr_y );
			glVertex2f( u[p], v[p] );
			
			glTexCoord2f( next_x, h-curr_y );
			glVertex2f( u[p1], v[p1] );
			
			glTexCoord2f( next_x, h-next_y );
			glVertex2f( u[p2], v[p2] );
			
			glTexCoord2f( curr_x, h-next_y );
			glVertex2f( u[p3], v[p3] );
			
			
			/*glTexCoord2f( u[p], v[p] );
			 glVertex3f( curr_x, curr_y, 0.0f );
			 glTexCoord2f( u[p1], v[p1] );
			 glVertex3f( next_x, curr_y, 0.0f );
			 glTexCoord2f( u[p2], v[p2] );
			 glVertex3f( next_x, next_y, 0.0f );
			 glTexCoord2f( u[p3], v[p3] );
			 glVertex3f( curr_x, next_y, 0.0f );*/
			
			curr_x = next_x;
			next_x += x_step;
		}
		
		curr_y = next_y;
		next_y += y_step;
	}
	
	glEnd();
	
	
	/*GLfloat tex_coords[] = {
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
	 glDisableClientState( GL_TEXTURE_COORD_ARRAY );*/
	
	glPopMatrix();
	glDisable(texData.textureTarget);
}

