#pragma once

#include "ofApp.h"

inline float getRadiusGlobal(int w= ofGetWidth(), int h= ofGetHeight()) {
    float r = 2.0f*(std::min(w, h) / 2)/3.0f;
    return r;
}

class TextTimer {
public:
    TextTimer(const std::string& textIn, float timeToRenderIn, float delay, float lineIn);
    void setup();
    void update();

    std::string& getPartialString();
    float getLine() { return line; }
    float timeToRender, timeBegan, timeDelay, lingerTime;

private:
    std::string& getRawText() { return rawText; }
    std::string rawText; // raw text
    std::string partialText;
    float line; // user define value
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

class Animate3d {
public:
    void setup(AnimRepeat repeat, float seconds, bool start=true);
    void update() { animatorUp.update(1.0f / ofGetTargetFrameRate()); animatorDown.update(1.0f / ofGetTargetFrameRate());};
    bool isAnimating() { return animatorUp.isAnimating() || animatorDown.isAnimating(); }
    ofxAnimatableFloat& getUpAnimator() { return animatorUp; }
    ofxAnimatableFloat& getDownAnimator() { return animatorDown; }

private:
    ofxAnimatableFloat animatorUp;
    ofxAnimatableFloat animatorDown;

};

// always knows it rotation coordindates
class SuperSphere : public ofSpherePrimitive, public Animate3d {
public:
    void setup(AnimRepeat repeat, float seconds, bool start, float x, float y, int w, int h);
    void draw();
    void home();
    void setRotation(const ofVec3f& r) { currentRotation = r; }
private:
    ofVec3f currentRotation;
};

class SuperCube : public ofBoxPrimitive, public Animate3d {
public:
    void setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h);
    void draw();
    void setRotation(const ofVec3f& r) { currentRotation = r; }
private:
    ofVec3f currentRotation;
};

class SuperCylinder: public ofCylinderPrimitive, public Animate3d {
public:
    void setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h);
    void draw();
    void setRotation(const ofVec3f& r) { currentRotation = r; }
private:
    ofVec3f currentRotation;
};


class Textures {
public:
    void setup(const std::string& path, float duration);
    void update();
    objectTexture&getCurrentRef();

private:
    void add(const std::string &name, const std::string &root);
    ofxAnimatableFloat selector; // pick eye to draw
    std::vector<objectTexture> skins;
};

class Sound {
public:
    Sound() { setup(); }

    void setup(float pitchIn = 0.0f, float ampIn = 0.0f, float triggerIn = 0.0f, float tempoIn = 108.f, int sequencerIn = 0);

    float pitch, amp, trigger, tempo;
    int sequencer;
    bool sendSound() { return soundSet; }
    void setSound(bool b) { soundSet = b; }

private:
    bool soundSet; // sent initial sound

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
    void setup();
    virtual void update() {};
    virtual void draw() {}

    bool isRunning() const { return running; } 
    void stop() { running = false; }
    int getID() { return id; }
    Sound&getSound() { return sound; }

protected:
    int id;
    Sound sound;
    void setupHelper(of3dPrimitive* primitive, SuperSphere *);
    ofRectangle rectangle;
    objectTexture texture;
    bool running;
    SuperSphere *parent;
};

class GameLevel {
public:
    enum Levels { NoGame = -1, Basic = 0, Medium = 1, Difficult = 2 };
    enum Durations { NoDuration = 0 , BasicDuration = 10, MediumDuration = 10, DifficultDuration = 10 };

    GameLevel(Levels level, Durations duration) { setup(level, duration); }

    bool inGame() { return getLevel() != NoGame; }
    bool advance(std::shared_ptr<GameLevel>&);
    std::shared_ptr<GameLevel> getNext();

    void  resetLevelTime() { gameLevelTime = ofGetElapsedTimef(); }
    Sound&getSound() { return sound; }

    Levels getLevel() { return level; }
    float getDuration() { return duration; } // run game for 30 seconds
    float timeLeft() { return duration - getLevelDuration(); }
    float getLevelDuration() { return ofGetElapsedTimef() - gameLevelTime; }
    void setup(Levels level, Durations duration);
    float gameLevelTime;
    float duration;
    Levels level;
    Sound sound;
};

const unsigned SphereGameItemTime = 10.0f;
class EyeGameItem : public GameItem {
public:
    EyeGameItem(const ofRectangle& rect, objectTexture texture, SuperSphere *parent, int id) :GameItem(rect, texture, id) { setup(parent, SphereGameItemTime); }
    virtual  ~EyeGameItem() {  }

    void setup(SuperSphere *parent, float duration);
    void update();
    void draw();
    bool isAnimating() { return sphere.isAnimating(); }

private:
    SuperSphere sphere;
};

const unsigned CubeGameItemTime = 1.0f;
class CubeGameItem : public GameItem {
public:
    CubeGameItem(const ofRectangle& rect, objectTexture texture, SuperSphere *parent, int id) :GameItem(rect, texture, id) { setup(parent, CubeGameItemTime); }
    virtual  ~CubeGameItem() {  }

    void setup(SuperSphere *parent, float duration);
    void update();
    void draw();

    bool isAnimating() { return cube.isAnimating(); }

private:
    SuperCube cube; 
};

const unsigned CylinderGameItemTime = 10.0f;
class CylinderGameItem : public GameItem {
public:
    CylinderGameItem(const ofRectangle& rect, objectTexture texture, SuperSphere *parent, int id) :GameItem(rect, texture, id) { setup(parent, CylinderGameItemTime); }
    virtual  ~CylinderGameItem() {  }

    void setup(SuperSphere *parent, float duration);
    void update();
    void draw();

    bool isAnimating() { return cylinder.isAnimating(); }

private:
    SuperCylinder cylinder;
};

// map location to interesting things
class LocationToActionMap : public ofRectangle {
public:
    LocationToActionMap() {
        c = 0; 
    }
    void draw();
    int c; // countid
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
    void update();
    void draw(Music*music);
    //http://www.findsounds.com/ISAPI/search.dll?keywords=cat
    void sounds(int duration= 5); // default to full sound
    void startPlaying();
    size_t winnerHitCount(); // count of items being animiated
    size_t winnerThreshold();
    bool isWinner() { return winnerThreshold() != (size_t)-1 && winnerHitCount() >= winnerThreshold(); } // easy mode! bugbug menu
    bool isAnimating();
    void windowResized(int w, int h);
    bool inGame() { return current->inGame(); }
    float w, h;
    ContoursBuilder contours;
    float maxForTrigger;

private:
    std::shared_ptr<GameLevel> current;// allocation not validated
    void removeGameItem(int id);
    void pushSphere(const ofRectangle&rect, int id);
    void pushCube(const ofRectangle&rect, int id);
    void pushCylinder(const ofRectangle&rect, int id);
    bool compute(LocationToActionMap* rect);
    TextEngine basicText;
    TextEngine fancyText;
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
    Textures cubesSkins; // cache images
    Textures spheresSkins;
    Textures cylindersSkins;
    Textures musicNotesSkins;
    void getCountours();
    void rotatingEyesDone(ofxAnimatableFloat::AnimationEvent & event);
    void fireWorks();
    float shapeMinSize;
    void buildTable();
    int squareCount;
    void buildX();
    void buildY();
    void rotate(const ofVec3f& target);
    std::vector<ofSoundPlayer> mySounds;
    bool sendFireworks;
    ofVec3f currentLocation;
    ofVec3f currentRotation;
    typedef std::pair<float, float> Key;
    std::map<Key, float> mapCameraInX; // range to rotation
    std::map<Key, float> mapCameraInY;
    std::map<std::pair<int, int>, LocationToActionMap> aimationMap; // map indexes, nullptr means no object found yet
    // convert to screen size
    float xFactor;
    float yFactor;
    std::list<std::shared_ptr<GameItem>> gameItems; // if you are in this list you have been found and not time out has occured bugbug add time out
    ofxAnimatableFloat blinker; // blink animation
};
