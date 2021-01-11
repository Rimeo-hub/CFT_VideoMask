#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"
#include "ofxKinectV2.h"
#include "ofxSyphon.h"

// send host (aka ip address)
#define HOST "172.20.10.11"

/// send port
#define PORT1 12000 // TouchDesigner
//#define PORT2 12000 // Unreal
//#define PORT3 14000 // Processing
//#define PORT4 16000 // Processing2

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxOscSender sender1;
    //ofxOscSender sender2;
    //ofxOscSender sender3;
    //ofxOscSender sender4;
    
    /*
     void DrawAlphImage();
     */
    
    //int xpos,ypos;
    int MaskingColor;
    int MaskingColorCount;
    int MaskingColorNum;
    int dpX;
    int dpY;
    
    
    //float threshold;
    
    ofFbo fbo;
    ofFbo fboDepth;     //Depth Image
    ofFbo masking;
    ofFbo BasicType;
    ofFbo BlueType;
    ofFbo RedType;
    ofFbo PhinkType;
    
    ofParameter<float> minArea, maxArea, threshold,Scale;
    ofParameter<int>Kinect_x[2],Kinect_y[2], blackX;
    ofParameter<bool> thresholdbool,UpandDown;
    ofxCv::ContourFinder contourFinder;
    
    ofImage image;      //Contain Image
    
    ofPixels pixels;
    ofxPanel panel;
    ofxPanel gui;
    
    vector < shared_ptr<ofxKinectV2> > kinects;
    vector <ofTexture> texDepth;
    
    ofParameter<bool>toggleGuiDraw = true;
    bool soundSwitch = true;
    int soundnum;
		
};
