#include "testApp.h"
#include "ofxRKProCamToolkit.h"
#include "ofxStringUtils.h"
#include "ofxXmlSettings.h"


void testApp::SayGoodbye(b2Joint* joint)
{
	if (m_mouseJoint == joint)
	{
		m_mouseJoint = NULL;
	}
	else
	{
		JointDestroyed(joint);
	}
}


// Implement contact listener.
void testApp::BeginContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    if (fixtureA->IsSensor() && !(fixtureB->IsSensor()))
    {
        sensors[fixtureA]->bodies.insert(fixtureB->GetBody());
        
    }
    
    if (fixtureB->IsSensor() && !(fixtureA->IsSensor()))
    {
        sensors[fixtureB]->bodies.insert(fixtureA->GetBody());
    }
    
    
}

// Implement contact listener.
void testApp::EndContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    if (fixtureA->IsSensor() && !(fixtureB->IsSensor()))
    {
        sensors[fixtureA]->bodies.erase(fixtureB->GetBody());
        
    }
    
    if (fixtureB->IsSensor() && !(fixtureA->IsSensor()))
    {
        sensors[fixtureB]->bodies.erase(fixtureA->GetBody());
    }
}


void testApp::addWord(wstring str) {
    
    
    
   
    b2AABB aabb,taabb;
    aabb.lowerBound.SetZero();
    aabb.upperBound.SetZero();
    
    for (int32 i = 0; i < m_groundBody->GetFixtureList()->GetShape()->GetChildCount(); ++i)
	{
		m_groundBody->GetFixtureList()->GetShape()->ComputeAABB(&taabb, m_groundBody->GetTransform(), i);
        aabb.Combine(taabb);
	}
    
    
    
    
    word w;
    w.str = str;
    float width = font.stringWidth(str)/REAL_PIXEL_PER_CENTIMETER;
    float height = font.stringHeight(str)/REAL_PIXEL_PER_CENTIMETER;
    
    
    
    cout << aabb.lowerBound.x << " " << aabb.lowerBound.y << " " << aabb.upperBound.x << " " << aabb.upperBound.y << " " << width << " " << height << endl;
    
    vector<word>::iterator witer;
    b2Fixture* fiter;
    
    b2PolygonShape shape;
    shape.SetAsBox(width/2.0, height/2.0);
    
    b2Vec2 pos;
    
    do {
       
        pos = b2Vec2(ofRandom(aabb.lowerBound.x+width/2,aabb.upperBound.x-width/2), ofRandom(aabb.lowerBound.y+height/2,aabb.upperBound.y-height/2));
        b2Transform trans(pos,b2Rot(0));
  
        for (witer=words.begin(); witer!=words.end(); witer++) {
            if (b2TestOverlap(&shape, 0, witer->body->GetFixtureList()->GetShape(), 0, trans,witer->body->GetTransform() )) {
                break;
            }
        }
        
        for (fiter = m_islands->GetFixtureList(); fiter; fiter = fiter->GetNext())
        {
            if (b2TestOverlap(&shape, 0, fiter->GetShape(), 0, trans,m_islands->GetTransform() )) {
                break;
            }
        }
        
    } while (witer!=words.end() || fiter);
 
        
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position =  pos;
    
    w.body = m_world->CreateBody(&bd);
    w.body->CreateFixture(&shape, 5.0f);
    
    words.push_back(w);
}


//--------------------------------------------------------------
void testApp::setup(){
    
    ofxXmlSettings xml;
    xml.loadFile("viewer.xml");
    xml.pushTag("viewer");
    
    
    
    tspsReceiver = new ofxTSPSReceiver();
	tspsReceiver->setListener( this );
	tspsReceiver->connect(PORT);
    
    
    
    SetLocale("he_IL.UTF-8");
    
    
    
    
    receiver.setup( xml.getAttribute("osc", "port", 12346));
    
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
    
    font.loadFont(xml.getAttribute("font", "filename", "Guttman Hatzvi-Normal.ttf"), xml.getAttribute("font", "size", 30),true,true);
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
    //	glEnable(GL_DEPTH_TEST);
	
	// this sets the camera's distance from the object
	
    // we need GL_TEXTURE_2D for our models coords.
    ofDisableArbTex();
    
    ofxAssimpModelLoader model;
    
    model.loadModel(xml.getAttribute("model", "city", ""));
    cityMesh = model.getMesh(0);
    model.loadModel(xml.getAttribute("model", "tent", ""));
	tentMesh = model.getMesh(0);
    
    

    
    xml.popTag();
    
    overlay.load();
    ofLoadMatrices(ofToDataPath("matrices.txt"),projMat,mvMat);
    
    
    screen &scr = overlay.playground;
    ofMatrix4x4 trans = ofMatrix4x4::getTransposedOf(ofMatrix4x4::newTranslationMatrix(scr.origin));
    // double rotation - align x axis and align new y yaxis
    ofMatrix4x4 rot = ofMatrix4x4::newRotationMatrix(scr.xVec,ofVec3f(1.0f,0.0f,0.0f));
    rot.postMult(ofMatrix4x4::newRotationMatrix(scr.yVec*rot,ofVec3f(0.0f,1.0f,0.0f)));
    
    ofMatrix4x4 scl = ofMatrix4x4::newScaleMatrix(-1.0f,1.0f,1.0f); 
    scr.glMat = ofMatrix4x4::getTransposedOf(trans*rot*scl);
    
    {
        
        
        m_world = new b2World(b2Vec2(0.0f,0.0f)); 
        
        m_world->SetDestructionListener(this);
        m_world->SetContactListener(this);
        m_world->SetDebugDraw(&m_debugDraw);
        
        uint32 flags = b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit ;
        
        //	flags += settings->drawJoints			* b2DebugDraw::e_jointBit;
        //	flags += settings->drawAABBs			* b2DebugDraw::e_aabbBit;
        //	flags += settings->drawPairs			* b2DebugDraw::e_pairBit;
        //	flags += settings->drawCOMs				* b2DebugDraw::e_centerOfMassBit;
        m_debugDraw.SetFlags(flags);
        
        
        b2BodyDef bd;
        m_groundBody = m_world->CreateBody(&bd);
        vector<b2Vec2> vs;
        
 
        for (vector<ofVec3f>::iterator iter=overlay.polygons.front().vertices.begin();iter!=overlay.polygons.front().vertices.end();iter++) {
            ofVec3f vec(*iter-overlay.playground.origin);
            vs.push_back(b2Vec2(overlay.playground.xVec.dot(vec),overlay.playground.yVec.dot(vec)));
        }
        
        
        b2ChainShape loop;
        loop.CreateLoop(vs.data(), vs.size());
        b2FixtureDef fd;
        fd.shape = &loop;
        fd.density = 0.0f;
        m_groundBody->CreateFixture(&fd);
        
       
        
       
        cout<<overlay.polygons.size()<<endl;
        m_islands = m_world->CreateBody(&bd);
        
        for (vector<polygon>::iterator piter=overlay.polygons.begin()+1; piter!=overlay.polygons.end(); piter++) {
            vs.clear();
            cout << piter->vertices.size() << endl;
            for (vector<ofVec3f>::iterator iter=piter->vertices.begin();iter!=piter->vertices.end();iter++) {
                ofVec3f vec(*iter-overlay.playground.origin);
                b2Vec2 vec2=b2Vec2(overlay.playground.xVec.dot(vec),overlay.playground.yVec.dot(vec));
                vs.push_back(vec2);
            }
           
            
            b2PolygonShape island;
            island.Set(vs.data(), vs.size());
            fd.shape = &island;
            fd.density = 0.0f;
            m_islands->CreateFixture(&fd);
            
        }
    }
    
        
    velocityIterations = 6;
	positionIterations = 2;
	timeStep = 1.0f/30.0f;
	m_stepCount = 0;
    m_mouseJoint = NULL;
    
    //coordinator.setup(ofGetWidth(), ofGetHeight(), ofPoint(ofGetWidth()/2,ofGetHeight()), 20);
    
    
    
    
    ofBackground(0);
    
    ofEnableSmoothing();
	
	// show a faint the non-curve version of the same polygon:
	ofEnableAlphaBlending();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
	    
    //some model / light stuff
    glShadeModel(GL_SMOOTH);
    light.enable();
    ofEnableSeparateSpecularLight();
    
        
    glEnable(GL_NORMALIZE);

    
    ofSetColor(150);
    
    bDrawDebug = true;
    bEasyCam = false;
    
}


b2Fixture* testApp::createSensor(ofxTSPSPerson* person) {
    vector<b2Vec2> vs;
    for(int c = 0; c < person->contour.size(); c++){
        vs.push_back(b2Vec2(person->contour[c].x*ofGetWidth()/REAL_PIXEL_PER_CENTIMETER,person->contour[c].y*ofGetHeight()/REAL_PIXEL_PER_CENTIMETER));
    }
    
    b2ChainShape loop;
    loop.CreateLoop(vs.data(), vs.size());
    b2FixtureDef fd;
    fd.shape = &loop;
    fd.isSensor = true;
    
    return m_groundBody->CreateFixture(&fd);
}


//--------------------------------------------------------------
void testApp::update(){
    
/*
    for(int i = 0; i < receiver->totalPeople(); i++){
		ofxTSPSPerson* person = receiver->personAtIndex(i);
		ofColor* color = (ofColor*)person->customAttributes;
		ofSetColor(color->r, color->g, color->b);
		for(int c = 1; c < person->contour.size(); c++){
			ofLine(person->contour[c-1].x*ofGetWidth(), person->contour[c-1].y*ofGetHeight(), 
				   person->contour[c  ].x*ofGetWidth(), person->contour[c  ].y*ofGetHeight());
		}
    }
*/
    
                
        
        
    
 
    
    
    m_world->Step(timeStep, velocityIterations, positionIterations);
	
	if (timeStep > 0.0f)
	{
		++m_stepCount;
	}
    
    	
	m_world->ClearForces();
    
//    for (map<b2Fixture*,sensor*>::iterator siter=sensors.begin();siter!=sensors.end();siter++) {
//        for (set<b2Body*>::iterator biter=siter->second->bodies.begin(); biter!=siter->second->bodies.end(); biter++) {
//            b2Vec2 d(siter->second->person->velocity.x,siter->second->person->velocity.y);
//            
//            //d.Normalize();
//            b2Vec2 F = 10.0f * d;
//            (*biter)->ApplyForce(F, (*biter)->GetPosition());
//        }
//    }
    
    while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        
		// check for mouse moved message
		if ( m.getAddress() == "/addWords" )
		{
			string lastMessage = m.getArgAsString(0);
            vector<string> words = ofSplitString(m.getArgAsString(0), " ");
            for (vector<string>::iterator iter=words.begin(); iter!=words.end(); iter++) {
                wstring word(StringToWString(*iter));
                reverse(word.begin(),word.end());
                addWord(word);
            } 
            
            
            cout << "received: " << lastMessage << endl;
            
            
		}
    }


    
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    glMatrixMode(GL_PROJECTION);
    ofPushMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    ofPushMatrix();
    
    
    if (bEasyCam) {
        cam.begin();
    } else {
        ofApplyMatrices(projMat, mvMat);
    }

    
    glEnable(GL_DEPTH_TEST);
    
    ofSetColor(50);
    cityMesh.drawFaces();
    
    glDisable(GL_DEPTH_TEST);
    ofPushMatrix();
    glMultMatrixf(overlay.playground.glMat.getPtr());
    
    if (bDrawDebug) {
        m_world->DrawDebugData();
        
        if (m_mouseJoint)
        {
            b2Vec2 p1 = m_mouseJoint->GetAnchorB();
            b2Vec2 p2 = m_mouseJoint->GetTarget();
            
            b2Color c;
            c.Set(0.0f, 1.0f, 0.0f);
            m_debugDraw.DrawPoint(p1, 4.0f, c);
            m_debugDraw.DrawPoint(p2, 4.0f, c);
            
            c.Set(0.8f, 0.8f, 0.8f);
            m_debugDraw.DrawSegment(p1, p2, c);
        }
    }
    
    
    
    
    
    
    
    ofSetColor(150);
    
    for (vector<word>::iterator iter=words.begin(); iter!=words.end(); iter++) {
        
        
        
        const b2Transform& xf = iter->body->GetTransform();
        
        //font.drawString(iter->first, (ofGetWidth()-messageWidth)/2, (ofGetHeight()-messageHeight)/2);
        ofPushMatrix();
        ofTranslate(xf.p.x, xf.p.y);
        ofScale(1.0/REAL_PIXEL_PER_CENTIMETER, 1.0/REAL_PIXEL_PER_CENTIMETER);
        ofRotate(xf.q.GetAngle()*180/M_PI);
        
        float messageWidth = font.stringWidth(iter->str);
        float messageHeight = font.stringHeight(iter->str);
        font.drawString(iter->str, -messageWidth/2,messageHeight/2 );
        ofPopMatrix();
        
    }
     
    
    ofPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
    
    ofSetColor(100);
    for (vector<tent>::iterator iter=overlay.tents.begin(); iter!=overlay.tents.end(); iter++) {
        ofPushMatrix();
        glMultMatrixf(iter->glMat.getPtr());
        
        tentMesh.drawFaces();
        
        ofPopMatrix();
    }
    
    if (bEasyCam) {
        cam.end();
    }
    
    glMatrixMode(GL_PROJECTION);
    ofPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    ofPopMatrix();
    
//    for(int i = 0; i < receiver->totalPeople(); i++){
//		ofxTSPSPerson* person = receiver->personAtIndex(i);
//		ofColor* color = (ofColor*)person->customAttributes;
//		ofSetColor(color->r, color->g, color->b);
//		for(int c = 1; c < person->contour.size(); c++){
//			ofLine(person->contour[c-1].x*ofGetWidth(), person->contour[c-1].y*ofGetHeight(), 
//				   person->contour[c  ].x*ofGetWidth(), person->contour[c  ].y*ofGetHeight());
//		}
//    }
    
    
}

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}
    
	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;
                
				// We are done, terminate the query.
				return false;
			}
		}
        
		// Continue the query.
		return true;
	}
    
	b2Vec2 m_point;
	b2Fixture* m_fixture;
};


void testApp::MouseDown(const b2Vec2& p)
{
	m_mouseWorld = p;
	
	if (m_mouseJoint != NULL)
	{
		return;
	}
    
	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;
    
	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	m_world->QueryAABB(&callback, aabb);
    
	if (callback.m_fixture)
	{
		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef md;
		md.bodyA = m_groundBody;
		md.bodyB = body;
		md.target = p;
		md.maxForce = 1000.0f * body->GetMass();
		m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&md);
		body->SetAwake(true);
	}
}





void testApp::MouseUp(const b2Vec2& p)
{
	if (m_mouseJoint)
	{
		m_world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}
	
	
}

void testApp::MouseMove(const b2Vec2& p)
{
	m_mouseWorld = p;
	
	if (m_mouseJoint)
	{
		m_mouseJoint->SetTarget(p);
	}
}


//called when the person enters the system
void testApp::personEntered( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
	ofColor* color = new ofColor();
	color->r = ofRandom(0, 255);
	color->g = ofRandom(0, 255);
	color->b = ofRandom(0, 255);
	color->a = 255;
	
	//put a color into the custom attributes field
	person->customAttributes = color;
    
    sensor* s= new sensor;
    s->person = person;
    sensors[createSensor(person)] = s;
        
	
}

//called each time the centroid moves (a lot)
void testApp::personMoved( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
    for (map<b2Fixture*,sensor*>::iterator iter=sensors.begin(); iter!=sensors.end(); iter++) {
        if (iter->second->person==person) {
            m_groundBody->DestroyFixture(iter->first);
            sensor* s= iter->second;
            sensors.erase(iter);
            sensors[createSensor(person)] = s;
            break;
        }
    }
}

//called one frame before the person is removed from the list to let you clean up
void testApp::personWillLeave( ofxTSPSPerson* person, ofxTSPSScene* scene )
{
	//delete the color so we free up memory.
	delete (ofColor*)person->customAttributes;
    for (map<b2Fixture*,sensor*>::iterator iter=sensors.begin(); iter!=sensors.end(); iter++) {
        if (iter->second->person==person) {
            m_groundBody->DestroyFixture(iter->first);
            sensor* s= iter->second;
            sensors.erase(iter);
            delete s;
            break;
        }
    }
    
    
}

//called every frame no matter what.
void testApp::personUpdated(ofxTSPSPerson* person, ofxTSPSScene* scene)
{
    
    
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    
    
    switch (key) {
        case 'F':
		case 'f':
			ofToggleFullscreen();
			break;
        case 'C':
		case 'c':
			bEasyCam=!bEasyCam;
			break;
        case 'D':
        case 'd':
            bDrawDebug = !bDrawDebug;
            break;
        case 'G':
        case 'g':
            if (bEasyCam) {
                cam.begin();
                ofSaveMatrices(ofToDataPath("matrices.txt"));
                cam.end();
            }
            break;
            
        case 'L':
        case 'l':
            
            ofLoadMatrices(ofToDataPath("matrices.txt"),projMat,mvMat);
            
            break;    
        
        case 'A':
        case 'a': {
            wstring src = L"חברתי";
            wstring word;
            for (wstring::reverse_iterator iter=src.rbegin(); iter!=src.rend(); iter++) {
                word.push_back(*iter);
            }
            addWord(word);
        } break;
            
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

#define MOUSE_TO_MODEL_SCALE 5
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    MouseMove(b2Vec2(x/MOUSE_TO_MODEL_SCALE,y/MOUSE_TO_MODEL_SCALE));
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    MouseDown(b2Vec2(x/MOUSE_TO_MODEL_SCALE,y/MOUSE_TO_MODEL_SCALE));
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    MouseUp(b2Vec2(x/MOUSE_TO_MODEL_SCALE,y/MOUSE_TO_MODEL_SCALE));
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