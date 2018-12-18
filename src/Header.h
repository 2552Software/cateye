#pragma once

#include "ofApp.h"
#include "sound.h"

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
    SuperSphere(const string&name) { setup(name); }
    void setup(const string&name);
    void update();
    void draw();
    Eye& getMainEye() { return eye; }
private:
    Eye eye;
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

class Eyes {
public:
    void setup(AnimRepeat repeat, float seconds, const std::string& path, float rotateIn);
    void update();
    void draw();
    void resize(int w, int h);
    bool isAnimating() {  return getAnimator().isAnimating();   }
    SuperSphere&getCurrentEyeRef() {   return eyes[(int)selector.getCurrentValue()];  }
    size_t count() { return eyes.size(); }
    ofxAnimatableFloat& getAnimator() { return animator; }
private:
    void add(const std::string &name, const std::string &root);
    ofxAnimatableFloat animator; // z direction
    ofxAnimatableFloat selector; // pick eye to draw
    std::vector<SuperSphere> eyes;
    float rotate;
};

class GameItem {
public:
    GameItem(const ofRectangle& rect, Eye eye, int level);
    bool operator==(const GameItem& rhs) const {
        return rectangle == rhs.rectangle;
    }
    bool operator==(const ofRectangle& rhs) const {
        return rectangle == rhs;
    }
    void setup();
    void update();
    void draw();
    bool isAnimating() { return animater.isAnimating(); }

private:
    int level;
    ofBoxPrimitive box; // pick a shape 
    ofRectangle rectangle;
    ofSpherePrimitive sphere;
    ofCylinderPrimitive cylinder; // like a coin -- for music notes
    Eye myeye;
    ofxAnimatableFloat animater; 
};

// map location to interesting things
class LocationToInfoMap : public ofRectangle {
public:
    LocationToInfoMap() {}
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
    void startPlaying();
    size_t winnerHitCount(); // count of items being animiated
    void clear();
    size_t winnerThreshold() { return screenToAnimationMap.size(); } // have to get them all
    void setTriggerCount(float count);
    void setShapeMinSize(float size) { shapeMinSize = size; };
    bool isWinner() { return winnerHitCount() >= winnerThreshold(); } // easy mode! bugbug menu
    bool drawText();
    bool isAnimating();
    void credits(bool signon = false);
    void drawGame();
    void windowResized(int w, int h);
    Eyes mainEyes;
    Eyes rotatingEyes;
    bool find(const ofRectangle& item) { return std::find(gameItems.begin(), gameItems.end(), item) != gameItems.end(); }
    void setCount(int count);
    bool inGame() { return level >= 0; }
    void blink();
    void setTitle();
    std::string sillyString();
    Music sound;
    
private:
    Eye cube;
    Eye sphere;
    Eye musicNote;
    ofTrueTypeFont font;
    int level;
    void getCountours();
    struct TextEvent {
        int i;
    };
    ofEvent<TextEvent> textFinished;
    void rotatingEyesDone(ofxAnimatableFloat::AnimationEvent & event);
    void creditsDone(TextEvent & event);
    void draw(const std::string& s, float x=0.0f, float y = 0.0f);
    std::vector<TextTimer> displayText;
    void fireWorks();
    float shapeMinSize;
    void buildTable();
    int squareCount;
    ContoursBuilder contours;
    void buildX();
    void buildY();
    float maxForTrigger;
    void rotate(const ofVec3f& target);
    std::vector<ofSoundPlayer> mySounds;
    bool sendFireworks;
    ofVec3f currentLocation;
    ofVec3f currentRotation;
    typedef std::pair<float, float> Key;
    std::map<Key, float> mapCameraInX; // range to rotation
    std::map<Key, float> mapCameraInY;
    std::map<std::pair<int, int>, LocationToInfoMap> screenToAnimationMap; // map indexes, nullptr means no object found yet
    // convert to screen size
    float xFactor;
    float yFactor;
    std::list<GameItem> gameItems; // if you are in this list you have been found and not time out has occured bugbug add time out
    ofxAnimatableFloat blinker; // blink animation
};
