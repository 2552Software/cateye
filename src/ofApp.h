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

const int cameraWidth = 640;// 320; // the motion image from the camera
const int cameraHeight = 480;//240;
#define DATAPATH "runtime"

#include "Header.h"

class ofApp : public ofBaseApp {

public:
    
    bool hideMenu;
    ofxPanel gui;
    ofxIntSlider squareCount;
    ofxFloatSlider maxForTrigger;
    ofxFloatSlider maxForShape;
    void squareCountChanged(int & squareCount);
    void triggerCountChanged(float & count);
    void shapeSizeChanged(float & size);
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
    void audioOut(ofSoundBuffer &outBuffer);
private:
    ofSoundStream soundStream;
    double wavePhase;
    double pulsePhase;
    float volume;
    float frequency=172.0f;
    std::mutex audioMutex;
    ofSoundBuffer lastBuffer;
    float rms;
};
