#pragma once
#include "ofApp.h"

class Light : public ofLight {
public:
    void setup();
    void update();
    void setOrientation(ofVec3f rot);
private:
    ofxAnimatableOfColor specularcolor;
    ofxAnimatableOfColor ambientcolor;
};

class Camera : public ofEasyCam {
public:
    void setup() {    setDistance(4286);   } // magic number
};

class Material : public ofMaterial {
public:
    Material() {  setup();  }
    void setup();
    void update() {     color.update(1.0f / ofGetTargetFrameRate());   }
private:
    ofxAnimatableOfColor color; // not used yet
    ;
};

class Eye : public ofTexture {
public:
    Eye() {}
    Eye(const string&name) {     setup(name);  }

    void setup(const string&texName);
    void update() {   }
    void start();
    void stop();
private:
    Material material;
};

// always knows it rotation coordindates
class SuperSphere : public ofSpherePrimitive {
public:
    const int maxListSize = 100;

    void setup(const string&name, const string&blinkPath);
    void update();
    void draw();
    bool blinkingEnabled;
private:
    Eye eye;
    std::vector<Eye> blink;
    ofxAnimatableFloat blinker; // blink animation
};

class ofxAnimatableQueueofVec3f {
public:
    const size_t maxListSize = 100;

    void setup() { startPlaying(); }
    void update();
    void addTransition(ofxAnimatableOfPoint targetValue);
    void insertTransition(ofxAnimatableOfPoint targetValue, bool forceNext);
    bool hasFinishedAnimating();
    ofxAnimatableOfPoint getCurrentValue();
    ofPoint getPoint();
    void append(const ofVec3f& target);
    void startPlaying() {  playing = true; }

protected:
    bool playing = false;
private:
    ofxAnimatableOfPoint currentAnimation;
    std::list<ofxAnimatableOfPoint> animSteps;
};

class ContoursBuilder {
public:
    void setup();
    // return true if updated
    void update();
    void draw(float cxScreen, float cyScreen);
    ofxCvContourFinder contourFinder;
    ofxCvContourFinder contourDrawer;
private:
    ofVideoGrabber video;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage, backgroundImage, grayDiff;
};

class Map {
public:
    Map() {
       set(0);  // actions can vary
    }
    void set(int action);
    void set(const ofRectangle& r);
    void setup();
    void update();
    void draw();
    int getAction() { return action; }

private:
    int action; // things like have the  cat noise when hit
    ofxAnimatableOfColor color; // revert to black when not animating
    ofRectangle rectangle;

};

class ImageAnimator {
public:
    void setup();
    void update();
    void draw();
    void drawContours(float cxScreen, float cyScreen);
    //http://www.findsounds.com/ISAPI/search.dll?keywords=cat
    void sounds();
    void circle();

    void windowResized(int w, int h) {
        for (SuperSphere&eye : eyes) {
            eye.setRadius(std::min(w, h));
        }
    }

    void startPlaying();
    SuperSphere& getCurrentEyeRef();
    void add(const std::string &name, const std::string &root);

private:
    void randomize();
    ContoursBuilder contours;
    void buildX();
    void buildY();
    void rotate(const ofVec3f& target);
    std::vector<ofSoundPlayer> mySounds;
    ofxAnimatableFloat animatorIndex;
    ofxAnimatableQueueofVec3f rotator;
    ofxAnimatableQueueofVec3f path; // path of image
    std::vector<SuperSphere> eyes;
    ofVec3f currentLocation;
    ofVec3f currentRotation;
    typedef std::pair<float, float> Key;
    std::map<Key, float> mapX; // range to rotation
    std::map<Key, float> mapY;
    std::map<std::pair<int, int>, Map> thingsToDo; // map indexes
};

