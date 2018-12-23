#pragma once

#include "ofApp.h"

enum Levels { NoGame = -1, Basic = 0, Medium = 1, Difficult = 2, EndGame = 3 };
inline float getRadiusGlobal(int w= ofGetWidth(), int h= ofGetHeight()) {
    return (std::min(w, h) / 2) - (std::min(w, h) / 2)*0.20f;
}

inline void setAnimatorHelper(ofxAnimatableFloat& animator, float seconds, AnimRepeat repeat) {
    animator.reset(0.0f);
    animator.setDuration(seconds);
    animator.setRepeatType(repeat);
    animator.setCurve(LINEAR);
}

class TextTimer {
public:
    TextTimer(const std::string& textIn, float timeToRenderIn, float delay, float lineIn);
    void setup();
    void update();

    std::string& getPartialString();
    static const bool isReadyToRemove(const TextTimer& item);
    float getLine() { return line; }

private:
    std::string& getRawText() { return rawText; }
    std::string rawText; // raw text
    std::string partialText;
    float line; // user define value
    float timeToRender, timeBegan, timeDelay, lingerTime;
    bool doneDrawing;
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

class objectTexture : public ofTexture {
public:
    objectTexture() { }
    objectTexture(const string&name) {  setup(name);  }

    void setup(const string&texName);
    void update() {   }
    void start();
    void stop();
private:
    Material material;
};

class ContoursBuilder {
public:
    void setup();
    // return true if updated
    void update();
    void draw(float w, float h, float z);
    ofxCvContourFinder contourFinder;
    ofxCvContourFinder contourDrawer;
private:
    ofVideoGrabber video;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage, backgroundImage, grayDiff;
};

// always knows it rotation coordindates
class SuperSphere : public ofSpherePrimitive {
public:
    void setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h);
    void update();
    void draw();
    void home();
    bool isAnimating() { return animator.isAnimating(); }
    void setRotation(const ofVec3f& r) { currentRotation = r; }
    ofxAnimatableFloat& getAnimator() { return animator; }
private:
    ofVec3f currentRotation;
    ofxAnimatableFloat animator;
};

class SuperCube : public ofBoxPrimitive {
public:
    void setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h);
    void update();
    void draw();
    bool isAnimating() { return animator.isAnimating(); }
    void setRotation(const ofVec3f& r) { currentRotation = r; }
    ofxAnimatableFloat& getAnimator() { return animator; }
private:
    ofVec3f currentRotation;
    ofxAnimatableFloat animator;
};

class SuperCylinder: public ofCylinderPrimitive {
public:
    void setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h);
    void update();
    void draw();
    bool isAnimating() { return animator.isAnimating(); }
    void setRotation(const ofVec3f& r) { currentRotation = r; }
    ofxAnimatableFloat& getAnimator() { return animator; }
private:
    ofVec3f currentRotation;
    ofxAnimatableFloat animator;
};


class Textures {
public:
    void setup(const std::string& path);
    void update();
    objectTexture&getCurrentRef() {   return skins[(int)selector.getCurrentValue()];  }

private:
    void add(const std::string &name, const std::string &root);
    ofxAnimatableFloat selector; // pick eye to draw
    std::vector<objectTexture> skins;
};

class GameItem {
public:
    GameItem(const ofRectangle& rect, objectTexture texture, int id);
    virtual ~GameItem() { }
    bool operator==(const ofRectangle& rhs) const {
        return rectangle == rhs;
    }
    bool operator==(const int rhs) const {
        return id == rhs;
    }
    virtual void setup() {};
    virtual void update() {};
    virtual void draw() {};
    static const bool isReadyToRemove(std::shared_ptr<GameItem> item) { return !item->isRunning(); }
    virtual Levels nextLevel() = 0;
    bool isRunning() const { return running; } 
    void stop() { running = false; }
    int id;

protected:
    void setupHelper(of3dPrimitive* primitive, ofNode *parent);
    ofRectangle rectangle;
    objectTexture texture;
    bool running;
};

class SphereGameItem : public GameItem {
public:
    SphereGameItem(const ofRectangle& rect, objectTexture texture, ofNode *parent, int id) :GameItem(rect, texture, id) { setup(parent); }
    virtual  ~SphereGameItem() {  }

    void setup(ofNode *parent);
    void update();
    void draw();
    virtual Levels nextLevel() { return Medium; }
    bool isAnimating() { return sphere.isAnimating(); }

private:
    SuperSphere sphere;
};

class CubeGameItem : public GameItem {
public:
    CubeGameItem(const ofRectangle& rect, objectTexture texture, ofNode *parent, int id) :GameItem(rect, texture, id) { setup(parent); }
    virtual  ~CubeGameItem() {  }

    void setup(ofNode *parent);
    void update();
    void draw();
    virtual Levels nextLevel() { return Difficult; }
    bool isAnimating() { return cube.isAnimating(); }

private:
    SuperCube cube; 
};

class CylinderGameItem : public GameItem {
public:
    CylinderGameItem(const ofRectangle& rect, objectTexture texture, ofNode *parent, int id) :GameItem(rect, texture, id) { setup(parent); }
    virtual  ~CylinderGameItem() {  }

    void setup(ofNode *parent);
    void update();
    void draw();
    virtual Levels nextLevel() { return EndGame; }
    bool isAnimating() { return cylinder.isAnimating(); }

private:
    SuperCylinder cylinder;
};

class MusicItem : public CylinderGameItem { //bugbug roate them?
public:
    MusicItem(const ofRectangle& rect, objectTexture texture, ofNode *parent, int id) :CylinderGameItem(rect, texture, parent, id) { }
    virtual ~MusicItem() {  }

    virtual Levels nextLevel() { return NoGame; }

    static bool isAkey(std::shared_ptr<GameItem>item) { return (item->id == 1); }
    static bool isGkey(std::shared_ptr<GameItem>item) { return (item->id == 5); }
    static bool isTkey(std::shared_ptr<GameItem>item) { return (item->id == 7); }
    static bool isKkey(std::shared_ptr<GameItem>item) { return (item->id == 9); }
};

// map location to interesting things
class LocationToInfoMap : public ofRectangle {
public:
    LocationToInfoMap() { c = 0; }
    int c; // count
};

class TextEngine {
public:
    TextEngine(int idIn = 0) { id = idIn; }
    void draw(float z);
    void setup(int fontsize);
    void update();

    void addFullScreenText(TextTimer txt) { fullScreenText.push_back(txt); }
    void addInlineText(TextTimer txt) { inlineText.push_back(txt); }

    void drawShapes(const std::string& s);

    bool isFullScreenAnimating() { return fullScreenText.size() > 0; }
    std::string sillyString();
    void print(const std::string& s, float x, float y, float z);

    ofTrueTypeFont font;

    void bind(std::function<void(int, bool)> fn) {
        callback = std::bind(fn, std::placeholders::_1, std::placeholders::_2);
    }

private:
    std::list<TextTimer> fullScreenText;
    std::list<TextTimer> inlineText;
    bool animateString(TextTimer& text, int x, int y);
    int id;
    std::function<void(int, bool)> callback;
    void call(bool bInline) {
        if (callback) {
            callback(id, bInline);
        }
    }
};

class Music;
class Game {
public:

    Game();

    void setup();
    void update(Music*music);
    void draw();
    //http://www.findsounds.com/ISAPI/search.dll?keywords=cat
    void sounds(int duration= 5); // default to full sound
    void startPlaying();
    size_t winnerHitCount(); // count of items being animiated
    size_t winnerThreshold();
    bool isWinner() { return winnerHitCount() >= winnerThreshold(); } // easy mode! bugbug menu
    bool isAnimating();
    void windowResized(int w, int h);
    bool inGame() { return gameLevel > NoGame; }
    float w, h;
    ContoursBuilder contours;

private:
    void pushSphere(const ofRectangle&rect, int id);
    void pushCube(const ofRectangle&rect, int id);
    void pushCylinder(const ofRectangle&rect, int id);
    void pushMusic(const ofRectangle&rect, int id);
    bool compute(LocationToInfoMap rect, Music*);
    TextEngine basicText;
    TextEngine fancyText;
    float getLevelDuration() { return ofGetElapsedTimef() - gameLevelTime; }
    void  resetLevelTime() { gameLevelTime = ofGetElapsedTimef(); }
    std::string levelString();
    bool find(const ofRectangle& item);
    void credits(bool signon = false);    void setTriggerCount(float count=50.0f);
    void setShapeMinSize(float size=100.0f) { shapeMinSize = size; };
    void setSquareCount(int count=15);
    void clear();
    void textDone(int, bool);
    void drawContours();
    void drawGame();
    bool drawText();
    void blink();
    void setTitle();
    Textures mainEyesSkins;
    SuperSphere mainEye;
    Textures rotatingEyesSkins;
    SuperSphere rotatingEye;

    float gameLevelTime; // in seconds
    float gameLastActivity; // in seconds
    Textures cubesSkins; // cache images
    Textures spheresSkins;
    Textures cylindersSkins;
    Textures musicNotesSkins;
    Levels gameLevel;
    void updateLevel();
    void getCountours(Music*music);
    void rotatingEyesDone(ofxAnimatableFloat::AnimationEvent & event);
    void fireWorks();
    float shapeMinSize;
    void buildTable();
    int squareCount;
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
    std::list<std::shared_ptr<GameItem>> gameItems; // if you are in this list you have been found and not time out has occured bugbug add time out
    ofxAnimatableFloat blinker; // blink animation
};
