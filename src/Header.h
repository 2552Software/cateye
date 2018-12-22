#pragma once

#include "ofApp.h"

enum Levels { NoGame = -1, Basic = 0, Medium = 1, Difficult = 2, EndGame = 3 };
inline float getRadius() {
    return std::min(ofGetWidth(), ofGetHeight()) / 2;
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

class EyeTexture : public ofTexture {
public:
    EyeTexture() { }
    EyeTexture(const string&name) {  setup(name);  }

    void setup(const string&texName);
    void update() {   }
    void start();
    void stop();
private:
    Material material;
};

// always knows it rotation coordindates
class SuperSphere {
public:
    SuperSphere(const string&name) { setup(name); }
    void setup(const string&name);
    void update();
    void draw();
    EyeTexture& getTexture() { return eye; }
    ofVec3f currentRotation;
    float getRadius() { return r; }
private:

    EyeTexture eye;
    float r;
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

class Eyes {
public:
    void setup(AnimRepeat repeat, float seconds, const std::string& path);
    void rotate(ofVec3f r);
    void update();
    void draw();
    bool isAnimating() {  return getAnimator().isAnimating();   }
    SuperSphere&getCurrentSphereRef() {   return eyes[(int)selector.getCurrentValue()];  }
    size_t count() { return eyes.size(); }
    ofxAnimatableFloat& getAnimator() { return animator; }
    EyeTexture& getEyeRef() { return getCurrentSphereRef().getTexture(); }
private:
    void add(const std::string &name, const std::string &root);
    ofxAnimatableFloat animator; // z direction
    ofxAnimatableFloat selector; // pick eye to draw
    std::vector<SuperSphere> eyes;
};

class GameItem {
public:
    GameItem(const ofRectangle& rect, EyeTexture eye, Levels level, int id);
    bool operator==(const GameItem& rhs) const {
        return rectangle == rhs.rectangle;
    }
    bool operator==(const ofRectangle& rhs) const {
        return rectangle == rhs;
    }
    bool operator==(const int rhs) const {
        return id == rhs;
    }
    void setup();
    void update();
    void draw();
    bool isAnimating() { return animater.isAnimating(); }
    int id;
    Levels level;
    static bool isMusicNote(const GameItem& item) { return (item.level == EndGame); }
    static bool isAkey(const GameItem& item) { return (item.id == 1); }
    static bool isGkey(const GameItem& item) { return (item.id == 5); }
    static bool isTkey(const GameItem& item) { return (item.id == 7); }
    static bool isKkey(const GameItem& item) { return (item.id == 9); }

private:
    ofBoxPrimitive box; // pick a shape 
    ofRectangle rectangle;
    ofSpherePrimitive sphere;
    ofCylinderPrimitive cylinder; // like a coin -- for music notes
    EyeTexture myeye;
    ofxAnimatableFloat animater; 
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
    void draw(float x, float y, float z);
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
    bool compute(LocationToInfoMap rect, Music*);
    TextEngine basicText;
    TextEngine fancyText;
    float getLevelDuration() { return ofGetElapsedTimef() - gameLevelTime; }
    void  resetLevelTime() { gameLevelTime = ofGetElapsedTimef(); }
    std::string levelString();
    bool find(const ofRectangle& item) { return std::find(gameItems.begin(), gameItems.end(), item) != gameItems.end(); }
    void credits(bool signon = false);    void setTriggerCount(float count=50.0f);
    void setShapeMinSize(float size=100.0f) { shapeMinSize = size; };
    void setSquareCount(int count=15);
    void clear();
    void textDone(int, bool);
    void drawContours();
    void drawRotatingEyes();
    void drawGame();
    bool drawText();
    void blink();
    void setTitle();
    Eyes mainEyes;
    Eyes rotatingEyes;
    float gameLevelTime; // in seconds
    float gameLastActivity; // in seconds
    Eyes cubes; // cache images
    Eyes spheres;
    Eyes cylinders;
    Eyes musicNotes;
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
    std::list<GameItem> gameItems; // if you are in this list you have been found and not time out has occured bugbug add time out
    ofxAnimatableFloat blinker; // blink animation
};
