#include "testApp.h"
#include "ofxXmlSettings.h"

#include "Poco/Net/MailMessage.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/StringPartSource.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Exception.h"

using Poco::Net::SMTPClientSession;
using Poco::Exception;

#define EXTENSION "jpg"
#define PROCESS_DELAY 60000


#define OFX_SMTP_PORT 25

string getTime() {
    time_t rawtime;
    time ( &rawtime );
    return string(ctime (&rawtime));
}


//--------------------------------------------------------------
void testApp::setup(){

	ofSetLogLevel(OF_LOG_NOTICE);


    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        xml.pushTag("settings");
        camWidth = xml.getAttribute("camera", "width", 1600);
        camHeight = xml.getAttribute("camera", "height", 1200);
        photoWidth = xml.getAttribute("photo", "width", 864.0);
        photoHeight = xml.getAttribute("photo", "height", 540.0);
        thumbWidth = xml.getAttribute("thumb", "width",120.0);
        thumbHeight = xml.getAttribute("thumb", "height", 75.0);
        lifetime = xml.getAttribute("photo", "lifetime", 20.0);
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(camWidth,camHeight);
        //xml.getAttribute("settings", "root", "");

        receiver.setup(xml.getAttribute("receiver", "port", 10000));
        sound.loadSound(xml.getAttribute("trigger", "sound", "SOUND_SEQ.aif"));
        delay = xml.getAttribute("trigger", "delay", 3000);

        server = ofxHTTPServer::getServer(); // get the instance of the server
        server->setServerRoot("");		 // folder with files to be served
        server->start(xml.getAttribute("server", "port", 8888));

        hostname = xml.getAttribute("trigger", "hostname", "tevet machine");
        baudrate = xml.getAttribute("trigger", "baudrate", 9600);

    }



    blinkCounter = 0;
    imageCounter = 0;

    bTrigger = false;
   //	videoInverted 	= new unsigned char[camWidth*camHeight*3];
//	videoTexture.allocate(camWidth,camHeight, GL_RGB);
    processTimer = ofGetElapsedTimeMillis();
    
    
    serialTimer = ofGetElapsedTimeMillis();
    bConnected = false;
    trigger();
   
}

bool myfunction (string s1,string s2) { return (atoi(s1.c_str())<atoi(s2.c_str())); }

//--------------------------------------------------------------
void testApp::update(){

	ofBackground(100,100,100);
    vidGrabber.grabFrame();
    
    for (map<string,sender>::iterator iter = senders.begin();iter!=senders.end();iter++) {
        if (iter->second.bConnected && ofGetElapsedTimeMillis() > iter->second.timer+60000) {
            iter->second.bConnected = false;
            cout << getTime() << iter->first << " is disconnected" << endl;
        }
    }
    
    if (!bConnected) {
        if (ofGetElapsedTimeMillis()>serialTimer) {
            cout << ".";
            bConnected=serial.connect(baudrate);
            if (bConnected) {
                cout << "\nconnected" << endl;
                mailAlert(hostname+": arduino is connected");
            }
            serialTimer = ofGetElapsedTimeMillis()+10000;
        }
        
    } else {
        
        if (blinkCounter && ofGetElapsedTimeMillis()>blinkTimer) {
            if (blinkCounter>1) {
                serial.writeBytes("l",1);
            } else
            {
                serial.writeBytes("f",1);
            }
            blinkCounter--;
            blinkTimer = ofGetElapsedTimeMillis()+1000;
        }
        
        string s;
        if (serial.readUntil(s, 'e')) {
            
            //cout << s ;
            serialTimer = ofGetElapsedTimeMillis()+1000;
            
            if (!bTrigger && s.length()>=2 && s[s.length()-2]=='1') {
                trigger();
            }
        
        } else {
            //cout << ".";
            if (ofGetElapsedTimeMillis()>serialTimer) {
                serialTimer = ofGetElapsedTimeMillis()+10000;
                bConnected = false;
                serial.close();
                mailAlert(hostname+": been there done that but the arduino was disconnected");
            }
        }
        
    }
    
    if (bTrigger && ofGetElapsedTimeMillis()>delayTimer ) {
        bTrigger = false;
        image.setFromPixels(vidGrabber.getPixelsRef());

        float scale = min((float)vidGrabber.getWidth()/photoWidth,(float)vidGrabber.getHeight()/photoHeight);
        int newWidth = floor(photoWidth*scale);
        int newHeight = floor(photoHeight*scale);
//        cout << newWidth << "\t" << newHeight << endl;
//
//        stringstream ss;
//        ss << "PHOTO_" << ofGetHours() << "_" << ofGetMinutes() << "_" << ofGetSeconds();
//        cout << ss.str() << endl;

        image.crop(0.5*(image.getWidth()-newWidth), 0.5*(image.getHeight()-newHeight), newWidth, newHeight);
        image.resize(photoWidth, photoHeight);
        unsigned char *data = image.getPixels(); // unsigned char


        int myDataLength = image.getWidth() * image.getHeight() * 3;

        for (int i = 0; i < myDataLength; i+=3)
        {
            unsigned char r_pixel = data[i];
            unsigned char g_pixel = data[i+1];
            unsigned char b_pixel = data[i+2];

            int outputRed = (r_pixel * .393) + (g_pixel *.769) + (b_pixel * .189);
            int outputGreen = (r_pixel * .349) + (g_pixel *.686) + (b_pixel * .168);
            int outputBlue = (r_pixel * .272) + (g_pixel *.534) + (b_pixel * .131);

            if(outputRed>255)outputRed=255;
            if(outputGreen>255)outputGreen=255;
            if(outputBlue>255)outputBlue=255;


            data[i] = outputRed;
            data[i+1] = outputGreen;
            data[i+2] = outputBlue;
        }

        string imageName = ofToString(imageCounter);
        imageCounter++;
        cout << "saving photo " << imageName << endl;
        
        image.saveImage("photos/"+imageName+"."+EXTENSION);
        image.resize(thumbWidth, thumbHeight);
        image.saveImage("thumbs/"+imageName+"."+EXTENSION);
        image.update();
        

        ofxOscMessage m;
        m.setAddress("/new");
        m.addStringArg(imageName);
        for (map<string,sender>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
            iter->second.osc->sendMessage(m);
        }

    }

//	if (vidGrabber.isFrameNew()){
//		int totalPixels = camWidth*camHeight*3;
//		unsigned char * pixels = vidGrabber.getPixels();
//		for (int i = 0; i < totalPixels; i++){
//			videoInverted[i] = 255 - pixels[i];
//		}
//		videoTexture.loadData(videoInverted, camWidth,camHeight, GL_RGB);
//	}

    // check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );

		
        if ( m.getAddress() == "/heartbeat" ) {
            string host = m.getRemoteIp();
            map<string,sender>::iterator iter = senders.find(host);
            if (iter==senders.end()) {
                
                int port = m.getArgAsInt32(0);
                
                sender s;
                
                s.osc = new ofxOscSender();
                s.osc->setup(host,port);
                s.timer=ofGetElapsedTimeMillis();
                s.bConnected = true;
                
                senders[host]=s;
               
                
                cout << getTime() <<"list for new client: " << host << "\t" << port << "\t";
                list(s.osc);
            } else {
                if (!iter->second.bConnected) {
                    iter->second.bConnected = true;
                    cout << getTime() << iter->first << " reconnected" << endl;
                }
                iter->second.timer = ofGetElapsedTimeMillis();
                
            }
		} else if ( m.getAddress() == "/list" ) {
            
            
            string host = m.getRemoteIp();
            map<string,sender>::iterator iter = senders.find(host);
            if (iter!=senders.end()) {
                cout << getTime() <<"list for existing client: " << host << "\t";
                list(iter->second.osc);
            }
            
		} else if ( m.getAddress() == "/delete" ) {
            string name = m.getArgAsString(0);
			ofFile file(ofToDataPath("photos/"+name+"."+EXTENSION));
            if (file.exists()) {
                file.remove();
            }

            file = ofFile(ofToDataPath("thumbs/"+name+"."+EXTENSION));

            if (file.exists()) {
                file.remove();
            }

            m.clear();
            m.setAddress("/remove");
            m.addStringArg(name);
            for (map<string,sender>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
                iter->second.osc->sendMessage(m);
            }

		}  

    }


    if (ofGetElapsedTimeMillis()-processTimer>PROCESS_DELAY) {
        
        
        processTimer = ofGetElapsedTimeMillis();

        ofDirectory dir(ofToDataPath("photos"));
        dir.listDir();
        for (int i=0;i<dir.size();i++) {
            float diff = difftime(time(NULL),dir.getFile(i).getPocoFile().getLastModified().epochTime()) / 60;
            //cout << dir.getName(i) << "\tdiff: " << diff << "\t" << endl ;
            if (diff>lifetime) {
                cout << getTime() << "deleting " << dir.getName(i) << endl;
                dir.getFile(i).remove();

                ofFile file(ofToDataPath("thumbs/"+dir.getName(i)));
                if (file.exists()) {
                    file.remove();
                }

                ofxOscMessage m;

                m.setAddress("/remove");
                m.addStringArg(ofSplitString(dir.getName(i), ".").front());
                for (map<string,sender>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
                    iter->second.osc->sendMessage(m);
                }

            }
        }

    }
}

void testApp::list(ofxOscSender *sender) {
    ofDirectory dir(ofToDataPath("photos"));
    dir.listDir();
    vector<string> names;
    
    for (int i=0;i<dir.size();i++) {
        names.push_back(ofSplitString(dir.getName(i), ".").front());
    }
    sort (names.begin(), names.end(), myfunction);

    for (vector<string>::iterator iter= names.begin(); iter!=names.end();iter++) {
        ofxOscMessage m;
        m.setAddress("/add");
        m.addStringArg(*iter);
        sender->sendMessage(m);
        cout << "\t" << *iter ;
    }
    cout << endl;

}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetHexColor(0xffffff);
    ofPushMatrix();
    float scale = ofGetWidth()/vidGrabber.getWidth();
    ofScale(scale, scale);
	vidGrabber.draw(0,0);
    ofPopMatrix();
    ofPushMatrix();
        if (image.getTextureReference().bAllocated()) {
        image.draw(40,40);
    }
    ofPopMatrix();
    ofSetHexColor(0x00ff00);
    ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate()), 10,10);
    
    
   
//	videoTexture.draw(20+camWidth,20,camWidth,camHeight);
}

void testApp::exit() {
    vidGrabber.close();
    for (map<string,sender>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
        delete iter->second.osc;
    }
    
    
}

void testApp::trigger() {
    cout << getTime() << "triggered...\t";
    delayTimer = ofGetElapsedTimeMillis()+3000;
    bTrigger = true;
    sound.play();
    blinkCounter = 4;
    blinkTimer = ofGetElapsedTimeMillis();
}

void testApp::mailAlert(string subject) {
    cout << getTime() << "sending mail alert" << endl;
    
    SMTPClientSession * session;
    
    try{
        
        session=new Poco::Net::SMTPClientSession("192.168.10.120",OFX_SMTP_PORT);
        session->login();
        
        
        Poco::Net::MailMessage message;
        message.setDate(Poco::Timestamp());
        message.setSender("noreply@towerofdavid.org.il");
        message.setSubject(subject);
        
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"roikr75@gmail.com"));
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"lofipeople@gmail.com"));
        message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,"yaal@roth-tevet.com"));
        session->sendMessage(message);
                
        if (session) {
            session->close();
            session = 0;
        }
    }catch(Poco::Exception e){
        ofLog(OF_LOG_ERROR,"cannot connect to the server");
        
        if (session) {
            session->close();
        }
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	// in fullscreen mode, on a pc at least, the
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...


	if (key == 's' || key == 'S'){
		vidGrabber.videoSettings();
	}

    if (key == ' '){
        trigger();
    }
}

void testApp::mousePressed(int x, int y, int button) {

}
