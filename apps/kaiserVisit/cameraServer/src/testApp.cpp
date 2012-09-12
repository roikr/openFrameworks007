#include "testApp.h"
#include "ofxXmlSettings.h"

#define EXTENSION "jpg"



//--------------------------------------------------------------
void testApp::setup(){

	serial.listDevices();
    //vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();



    ofxXmlSettings xml;
    if (xml.loadFile("settings.xml")) {
        camWidth = xml.getAttribute("settings", "width", 1280);
        camHeight = xml.getAttribute("settings", "width", 720);
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(camWidth,camHeight);
        //xml.getAttribute("settings", "root", "");
        xml.pushTag("settings");
        receiver.setup(xml.getAttribute("receiver", "port", 10000));
        sound.loadSound(xml.getAttribute("trigger", "sound", ""));
        delay = xml.getAttribute("trigger", "delay", 3000);

        server = ofxHTTPServer::getServer(); // get the instance of the server
        server->setServerRoot("");		 // folder with files to be served
        server->start(xml.getAttribute("server", "port", 8888));

        bSerial = serial.setup(xml.getAttribute("trigger", "portname", "/dev/tty.usbmodemfd121"), xml.getAttribute("trigger", "baudrate", 9600));
        if (bSerial) {
            serial.writeByte('r');
        }
    }





    blinkCounter = 0;

    bTrigger = false;
   //	videoInverted 	= new unsigned char[camWidth*camHeight*3];
//	videoTexture.allocate(camWidth,camHeight, GL_RGB);
}


//--------------------------------------------------------------
void testApp::update(){

	ofBackground(100,100,100);

	vidGrabber.grabFrame();

    if (bSerial) {

        if (serial.available()) {
            switch (serial.readByte()) {
                case OF_SERIAL_ERROR:
                    cout << "serial error" << endl;
                    break;
                case OF_SERIAL_NO_DATA:
                    break;
                default:
                    trigger();
                    break;
            }
        }


        if (blinkCounter && ofGetElapsedTimeMillis()>blinkTimer) {

            serial.writeByte('l');
            blinkCounter--;
            blinkTimer = ofGetElapsedTimeMillis()+1000;
            if (!blinkCounter) {
                serial.writeByte('r');
            }
        }
    }

    if (bTrigger && ofGetElapsedTimeMillis()>delayTimer ) {
        bTrigger = false;
        image.setFromPixels(vidGrabber.getPixelsRef());

        float width = 800.0;
        float height = 600.0;
        float scale = min((float)vidGrabber.getWidth()/width,(float)vidGrabber.getHeight()/height);
        int newWidth = floor(width*scale);
        int newHeight = floor(height*scale);
        cout << newWidth << "\t" << newHeight << endl;

        stringstream ss;
        ss << "PHOTO_" << ofGetHours() << "_" << ofGetMinutes() << "_" << ofGetSeconds();
        cout << ss.str() << endl;

        image.crop(0.5*(image.getWidth()-newWidth), 0.5*(image.getHeight()-newHeight), newWidth, newHeight);
        image.resize(width, height);
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


        image.saveImage("photos/"+ss.str()+"."+EXTENSION);
        image.resize(120, 90);
        image.saveImage("thumbs/"+ss.str()+"."+EXTENSION);
        image.update();

        ofxOscMessage m;
        m.setAddress("/new");
        m.addStringArg(ss.str());
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
            for (int i=0;i<dir.size();i++) {
                ofxOscMessage m;
                m.setAddress("/add");
                m.addStringArg(ofSplitString(dir.getName(i), ".").front());
                sender->sendMessage(m);
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
    blinkCounter = 3;
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

    if (key == ' ' && !bTrigger) {
        trigger();
    }
}

