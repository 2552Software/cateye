#pragma once
#include <algorithm>
#include <random>
#include "ofMain.h"
#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"
#include "ofxAnimatableOfColor.h"
#include "ofxAnimatableQueue.h"
#include "ofxOpenCv.h"
#include <ofxGui.h>

const int imgWidth = 640;// 320; // the motion image from the camera
const int imgHeight = 480;//240;
#define DATAPATH "runtime"

#include "Header.h"

class ofApp : public ofBaseApp {

public:
    
    bool hideMenu;
    ofxPanel gui;
    ofxIntSlider squareCount;
    void squareCountChanged(int & squareCount);

    Light	light;
    Camera camera;
    ImageAnimator eyeAnimator;
    ofMesh m;
    void setup();
    void update();
    void draw();
    //--------------------------------------------------------------
    void windowResized(int w, int h) {
        eyeAnimator.windowResized(w, h);
    }
    void keyPressed(int key);
private:
};
