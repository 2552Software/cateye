#pragma once
#include <algorithm>
#include <random>
#include <functional>
#include "ofMain.h"
#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"
#include "ofxAnimatableOfColor.h"
#include "ofxAnimatableQueue.h"
#include "ofxOpenCv.h"
#include <ofxGui.h>


const int cameraWidth = 640;// 320; // the motion image from the camera
const int cameraHeight = 480;//240;
#define EYES "eyes"
#define SPIRALS "spirals"
#define SOUNDS "sounds"
#define CUBES "cubes"
#define SPHERES "spheres"
#define MUSICNOTES "musicnotes"
#define CYLINDERS "cylinders"

#include "Header.h"

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();

    bool hideMenu;
    ofxPanel gui;
    ofxIntSlider squareCount;
    ofxFloatSlider maxForTrigger;
    ofxFloatSlider maxForShape;
    void squareCountChanged(int & squareCount);
    void triggerCountChanged(float & count);
    void shapeSizeChanged(float & size);
    Music *music;
    Light light;
    Camera camera;
    Game eyeAnimator;
    ofMesh m;
    void windowResized(int w, int h);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:
   
};
