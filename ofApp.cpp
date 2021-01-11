#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetWindowTitle("KinectDataExport");
    ofSetFrameRate(60); // run at 60 fps
    ofSetVerticalSync(true);
    
    // open an outgoing connection to HOST:PORT
    sender1.setup(HOST, PORT1);
    //sender2.setup(HOST, PORT2);
    //sender3.setup(HOST, PORT3);
    //sender4.setup(HOST, PORT4);
    
    //ofSetBackgroundAuto(false);
    
    ofBackground(0);
    MaskingColorCount = 0;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*-----------------------FboSetting-----------------------*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    fboDepth.allocate(ofGetWidth(), ofGetHeight());
    masking.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    image.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_GRAYSCALE);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Kinect setting and see how many devices we have.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    ofxKinectV2 tmp;
    vector <ofxKinectV2::KinectDeviceInfo> deviceList = tmp.getDeviceList();
    
    //allocate for this many devices
    kinects.resize(deviceList.size());
    texDepth.resize(kinects.size());
    //texRGB.resize(kinects.size());//RgbCamera Setting
    
    panel.setup("", "settings2.xml", 10, 100);
    
    //Note you don't have to use ofxKinectV2 as a shared pointer, but if you want to have it in a vector ( ie: for multuple ) it needs to be.
    for(int d = 0; d < kinects.size(); d++){
        kinects[d] = shared_ptr <ofxKinectV2> (new ofxKinectV2());
        kinects[d]->open(deviceList[d].serial);
        panel.add(kinects[d]->params);
    }
    
    /////////////////////////////////////////////////////////////
    /*---------------------OPCV GUI setting--------------------*/
    /////////////////////////////////////////////////////////////
    
    panel.loadFromFile("settings2.xml");
    gui.setup();
    gui.add(minArea.set("Min area", 5.995, 1, 1000));
    gui.add(maxArea.set("Max area", 72.955, 1, 1600));
    gui.add(threshold.set("Threshold", 56.25, 0, 250));
    gui.add(blackX.set("blackX", 56.25, 0, 250));
    gui.add(Kinect_x[0].set("KinectA_x", 0,-1024,ofGetWidth()));
    gui.add(Kinect_y[0].set("KinectA_y", 0,-1024,ofGetHeight()));
    gui.add(Kinect_x[1].set("KinectB_x", 512,-1024,ofGetWidth()));
    gui.add(Kinect_y[1].set("KinectB_y", 0,-1024,ofGetHeight()));
    gui.add(Scale.set("Scale", 1,0,10));
    gui.add(thresholdbool.set("threshold", true));
    gui.add(UpandDown.set("UpandDown", false));
    
    /////////////////////////////////////////////////////////////
    /*----------------OPCV Container setting-------------------*/
    /////////////////////////////////////////////////////////////
    
    // wait for half a frame before forgetting something
    contourFinder.getTracker().setPersistence(15);
    // an object can move up to 32 pixels per frame
    contourFinder.getTracker().setMaximumDistance(32);
    
    
    //    mainOutputSyphonServer.setName("Screen Output");
    kinects[0]->maxDistance = 1616.73;
    kinects[0]->minDistance = 1591.84;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    for(int d = 0; d < kinects.size(); d++){
        kinects[d]->update();
        if( kinects[d]->isFrameNew() ){
            texDepth[d].loadData( kinects[d]->getDepthPixels() );
        }
    }
    
    /////////////////////////////////////////////////////////////
    /*-------------------drowing KinectImage-------------------*/
    /////////////////////////////////////////////////////////////
    
    fboDepth.begin();
    ofBackground(255);
    if (UpandDown == true) {
        for(int d = 0; d < kinects.size(); d++){
            //texDepth[d].draw(0, 0,256,212);//half Size
            texDepth[d].draw(Kinect_x[d], Kinect_y[d],512 * Scale,424 * Scale);//normal Size
            //texDepth[d].draw(0, -40,1024,848);//normal Size
            //texRGB[d].loadData( kinects[d]->getRgbPixels() );//RGB Camera
        }
    }else{
        for(int d = kinects.size()-1; d > -1; d--){
            //texDepth[d].draw(0, 0,256,212);//half Size
            texDepth[d].draw(Kinect_x[d], Kinect_y[d],512 * Scale,424 * Scale);//normal Size
            //texDepth[d].draw(0, -40,1024,848);//normal Size
            //texRGB[d].loadData( kinects[d]->getRgbPixels() );//RGB Camera
        }
    }
    //ofSetColor(0,0,0);
    //ofDrawRectangle(0, ofGetHeight()-blackX, ofGetWidth(), ofGetHeight());
    
    fboDepth.end();
    
    
    //----------------------------------------------------------------
    //-------------------         CvCounter        -------------------
    //----------------------------------------------------------------
    fboDepth.readToPixels(pixels);       //read pixel form fboDepth(depthImage)
    image.setFromPixels(pixels);         //put the fboDepth pixel to image
    //grayImage.setFromPixels(pixels);
    if(thresholdbool == true)ofxCv::threshold(image, threshold);
    image.update();
    contourFinder.setMinAreaRadius(minArea);
    contourFinder.setMaxAreaRadius(maxArea);
    //contourFinder.setThreshold(threshold2);
    contourFinder.findContours(image);   //put the image to OFCV ContourFinder
    
    //grayImage.threshold(threshold);
    //CVcontourFinder.findContours(grayImage, 0, (512*424)/3, 10, true);
    // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
    // also, find holes is set to true so we will get interior contours as well....
    
    // find holes
    MaskingColor = contourFinder.size();
    soundnum=contourFinder.size();
    ofxCv::RectTracker& tracker = contourFinder.getTracker();
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255, 255, 255);
    ofxOscMessage m;
    //BGM.setLoop(true);
    
    /////////////////////////////////////////////////////////////
    /*---------------------  Graphic draw  --------------------*/
    /////////////////////////////////////////////////////////////
    //fboDepth.draw(0,0);
    //    mainOutputSyphonServer.publishScreen();
    //ofHideCursor();
    if(toggleGuiDraw == true){
        ofShowCursor();
        image.draw(0,0);//kinect setup
        //grayImage.draw(0,0);//kinect setup
        contourFinder.draw();
        //cout << MaskingColorNum;
        //cout << ofGetFrameRate() << endl;
        gui.draw();
        panel.draw();
        
        for(int i = 0; i < contourFinder.size(); i++) {
            ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
            int label = contourFinder.getLabel(i);
            
            m.setAddress("/position");
            //m.addFloatArg(ofMap(center.x, 0, ofGetWidth(), 0.f, 1.f, true));
            //m.addFloatArg(ofMap( center.y, 0, ofGetHeight(), 0.f, 1.f, true));
            m.addFloatArg(center.x);
            m.addFloatArg(center.y);
            
            ofSetColor(255, 0, 0);
            ofDrawCircle(center.x, center.y, 10,10);
            ofSetColor(255, 255, 255);
        }
        if(contourFinder.size() > 0){
            sender1.sendMessage(m, false);
            //sender2.sendMessage(m, false);
            //sender3.sendMessage(m, false);
            //sender4.sendMessage(m, false);
        }else{
            
            m.setAddress("/position");
            m.addFloatArg(-60);
            m.addFloatArg(-60);
            /*m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);
            m.addFloatArg(0);*/
            sender1.sendMessage(m, false);
            //sender2.sendMessage(m, false);
            //sender3.sendMessage(m, false);
            //sender4.sendMessage(m, false);
        }
        
        ofDrawBitmapString(ofToString(ofGetFrameRate())+"fps", 900, 15);
        ofDrawBitmapString(ofToString(dpX)+"dpX", 900, 25);
        ofDrawBitmapString(ofToString(dpY)+"dpY", 900, 35);
        ofDrawBitmapString(ofToString(soundnum)+"soundnum", 900, 45);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
     if(key == 's'){
        gui.saveToFile("settings.xml");
         panel.saveToFile("settings2.xml");
     }else if(key == 'l'){
         gui.loadFromFile("settings.xml");
         panel.loadFromFile("settings2.xml");
     }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
