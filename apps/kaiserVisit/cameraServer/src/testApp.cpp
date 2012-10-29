#include "testApp.h"
#include "ofxXmlSettings.h"

#define EXTENSION "jpg"
#define PROCESS_DELAY 60000


//--------------------------------------------------------------
void testApp::setup(){

	//serial.listDevices();
    //vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();


    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        xml.pushTag("settings");
        camWidth = xml.getAttribute("camera", "width", 1600);
        camHeight = xml.getAttribute("camera", "height", 1200);
        photoWidth = xml.getAttribute("photo", "width", 800.0);
        photoHeight = xml.getAttribute("photo", "height", 600.0);
        thumbWidth = xml.getAttribute("thumb", "width",120.0);
        thumbHeight = xml.getAttribute("thumb", "height", 75.0);
        lifetime = xml.getAttribute("photo", "lifetime", 4.0);
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(camWidth,camHeight);
        //xml.getAttribute("settings", "root", "");

        receiver.setup(xml.getAttribute("receiver", "port", 10000));
        sound.loadSound(xml.getAttribute("trigger", "sound", "SOUND_SEQ.aif"));
        delay = xml.getAttribute("trigger", "delay", 3000);

        server = ofxHTTPServer::getServer(); // get the instance of the server
        server->setServerRoot("");		 // folder with files to be served
        server->start(xml.getAttribute("server", "port", 8888));


        serial = new ofxSerial(xml.getAttribute("trigger", "portname", "/dev/ttyACM0"), xml.getAttribute("trigger", "baudrate", 9600));

    } else {
        serial = 0;
    }



    blinkCounter = 0;
    imageCounter = 0;

    bTrigger = false;
   //	videoInverted 	= new unsigned char[camWidth*camHeight*3];
//	videoTexture.allocate(camWidth,camHeight, GL_RGB);
    processTimer = ofGetElapsedTimeMillis();


}

bool myfunction (string s1,string s2) { return (atoi(s1.c_str())<atoi(s2.c_str())); }

//--------------------------------------------------------------
void testApp::update(){

	ofBackground(100,100,100);

	vidGrabber.grabFrame();

    if (serial) {

        string str;
        if (serial->readUntil(str,'t')) {
            cout << str << endl;
            if (!bTrigger) {
                trigger();
            }

        }



        if (blinkCounter && ofGetElapsedTimeMillis()>blinkTimer) {
            if (blinkCounter>1) {
                serial->writeBytes("l",1);
            } else
            {
                serial->writeBytes("f",1);
            }
            blinkCounter--;
            blinkTimer = ofGetElapsedTimeMillis()+1000;
        }
    }

    if (bTrigger && ofGetElapsedTimeMillis()>delayTimer ) {
        bTrigger = false;
        image.setFromPixels(vidGrabber.getPixelsRef());

        float scale = min((float)vidGrabber.getWidth()/photoWidth,(float)vidGrabber.getHeight()/photoHeight);
        int newWidth = floor(photoWidth*scale);
        int newHeight = floor(photoHeight*scale);
        cout << newWidth << "\t" << newHeight << endl;
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
        cout << "saving photo " << imageCounter << endl;
        image.saveImage("photos/"+imageName+"."+EXTENSION);
        image.resize(thumbWidth, thumbHeight);
        image.saveImage("thumbs/"+imageName+"."+EXTENSION);
        image.update();

        ofxOscMessage m;
        m.setAddress("/new");
        m.addStringArg(imageName);
        for (map<string,ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
            iter->second->sendMessage(m);
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

		// check for mouse moved message
		if ( m.getAddress() == "/delete" ) {
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
            for (map<string,ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
                iter->second->sendMessage(m);
            }

		}  else if ( m.getAddress() == "/list" ) {

            ofxOscSender *sender;

            string host = m.getRemoteIp();
            if (senders.find(host)==senders.end()) {

                int port = m.getArgAsInt32(0);
                sender = new ofxOscSender();
                sender->setup(host,port);

                senders[host]=sender;

                cout <<"new client: " << host << "\t" << port << endl;
            } else {
                sender = senders[host];
                cout <<"existing client: " << host << endl;
            }




            ofDirectory dir(ofToDataPath("photos"));
            dir.listDir();
            vector<string> names;
            
            for (int i=0;i<dir.size();i++) {
                names.push_back(ofSplitString(dir.getName(i), ".").front());
            }
            
            sort (names.begin(), names.end(), myfunction); 

            
            cout << "list" << endl;
            for (vector<string>::iterator iter= names.begin(); iter!=names.end();iter++) {
                ofxOscMessage m;
                m.setAddress("/add");
                m.addStringArg(*iter);
                sender->sendMessage(m);
                cout << "\t" << *iter << endl;
            }

		}

    }


    if (ofGetElapsedTimeMillis()-processTimer>PROCESS_DELAY) {
        cout << "processing" << endl;
        processTimer = ofGetElapsedTimeMillis();

        ofDirectory dir(ofToDataPath("photos"));
        dir.listDir();
        for (int i=0;i<dir.size();i++) {
            float diff = difftime(time(NULL),dir.getFile(i).getPocoFile().getLastModified().epochTime()) / 60;
            cout << dir.getName(i) << "\tdiff: " << diff << "\t" << endl ;
            if (diff>lifetime) {
                dir.getFile(i).remove();

                ofFile file(ofToDataPath("thumbs/"+dir.getName(i)));
                if (file.exists()) {
                    file.remove();
                }

                ofxOscMessage m;

                m.setAddress("/remove");
                m.addStringArg(ofSplitString(dir.getName(i), ".").front());
                for (map<string,ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
                    iter->second->sendMessage(m);
                }

            }
        }

    }
}



//--------------------------------------------------------------
void testApp::draw(){
	ofSetHexColor(0xffffff);
    ofPushMatrix();
    float scale = ofGetWidth()/vidGrabber.getWidth();
    ofScale(scale, scale);
	vidGrabber.draw(0,0);
    if (image.getTextureReference().bAllocated()) {
        image.draw(40,40);
    }
    ofPopMatrix();
//	videoTexture.draw(20+camWidth,20,camWidth,camHeight);
}

void testApp::exit() {
    for (map<string,ofxOscSender*>::iterator iter=senders.begin(); iter!=senders.end(); iter++) {
        delete iter->second;
    }
}

void testApp::trigger() {
    delayTimer = ofGetElapsedTimeMillis()+3000;
    bTrigger = true;
    sound.play();
    blinkCounter = 4;
    blinkTimer = ofGetElapsedTimeMillis();
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
