#pragma once

#include "ofApp.h"

class TextTimer {
public:
    TextTimer(const std::string& textIn, float timeToRenderIn, float delay, float lineIn);
    void setup();
    void update() { lasting.update(1.0f / ofGetTargetFrameRate()); }
    bool getString(std::string& text);
    bool isRunningOrWaitingToRun() {return !done || timeDelay; }
    ofxAnimatableOfColor lasting; // how long to draw after intial animation
    std::string text;
    float holdTextTime;
    float line;
private:
    int timeToRender, timeBegan, timeDelay;
    bool  timeSet, done;
};

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
    Map() {}
    Map(const ofRectangle& rectangle) {
       set(0);  // actions can vary
       set(rectangle);
    }
    void trigger();
    bool isAnimating() { return action > 0 && game.isAnimating();   }
    void set(int action);
    void setup();
    void update();
    void draw(int alpha=255);
    void reset() { game.reset(); }
    int getAction() { return action; }
    bool match(const ofRectangle& rect) { return rectangle.inside(rect); }//return rectangle.intersects(rect); }
    // convert to screen size
    float xFactor;
    float yFactor;

private:
 
    void set(const ofRectangle& rectangle);
    int action; // things like have the  cat noise when hit
    ofxAnimatableOfColor color; // revert to black when not animating
    ofRectangle rectangle;
    ofxAnimatableFloat game;
};

class Eyes {
public:
    void setup(AnimRepeat repeat, float seconds, const std::string& path, bool blink, float rotateIn);
    void update();
    void draw();
    void resize(int w, int h);
    bool isAnimating() {  return getAnimator().isAnimating();   }
    SuperSphere&getCurrentEyeRef() {   return eyes[(int)selector.getCurrentValue()];  }
    size_t count() { return eyes.size(); }
    ofxAnimatableFloat& getAnimator() { return animator; }
private:
    void add(const std::string &name, const std::string &root, bool blink);
    ofxAnimatableFloat animator; // z direction
    ofxAnimatableFloat selector; // pick eye to draw
    std::vector<SuperSphere> eyes;
    float rotate;
};

class ImageAnimator {
public:
    ImageAnimator();

    void setup();
    void update();
    void draw();
    void drawContours(float cxScreen, float cyScreen);
    //http://www.findsounds.com/ISAPI/search.dll?keywords=cat
    void sounds(int duration= 5); // default to full sound
    void circle();
    void windowResized(int w, int h);
    void startPlaying();
    int count(); // count of items being animiated
    void reset();
    void setCount(int count);
    void ignight(bool on=true);
    int  firstMatchCount() { return 1; } // intial game trigger bugbug make menu item
    int  winnerCount() { return 5; } // intial game trigger bugbug make menu item
    void setTriggerCount(float count);
    void setShapeMinSize(float size) { shapeMinSize = size; };
    bool isIgnighted(int count) { return count > firstMatchCount(); }
    bool isWinner(int count) { return count >= winnerCount(); } // easy mode! bugbug menu
    bool drawText();
    bool isAnimating();
    void credits(bool signon = false);
    void drawGame();
private:
    struct TextEvent {
        int i;
    };
    ofEvent<TextEvent> textFinished;
    void rotatingEyesDone(ofxAnimatableFloat::AnimationEvent & event);
    void creditsDone(TextEvent & event);
    void draw(const std::string& s, float x=0.0f, float y = 0.0f);
    std::vector<TextTimer> creditsText;
    ofTrueTypeFont font; 
    int level;
    void fireWorks();
    float shapeMinSize;
    void buildTable();
    int squareCount;
    void randomize();
    ContoursBuilder contours;
    void buildX();
    void buildY();
    float maxForTrigger;
    void rotate(const ofVec3f& target);
    std::vector<ofSoundPlayer> mySounds;
    Eyes mainEyes;
    Eyes rotatingEyes;
    bool sendFireworks;
    ofxAnimatableQueueofVec3f rotator;
    ofxAnimatableQueueofVec3f imagPath; // not used yet moves eye around
    ofVec3f currentLocation;
    ofVec3f currentRotation;
    typedef std::pair<float, float> Key;
    std::map<Key, float> mapCameraInX; // range to rotation
    std::map<Key, float> mapCameraInY;
    std::map<std::pair<int, int>, Map> thingsToDo; // map indexes
};
class Scheduler : public ofThread {
public:
    Scheduler() {
        timer.setPeriodicEvent((uint64_t)1000000000 * 60); // this is 1 second in nanoseconds
        startThread();
    }

private:
    ofTimer timer;
    void threadedFunction() {
        while (isThreadRunning()) {
            timer.waitNext();
            // Do your thing here. It will run once per 60 seconds.
        }
    }
};

