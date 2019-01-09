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

class Animate3d {
public:
    Animate3d(AnimRepeat repeat, float seconds) {
        setup(repeat, seconds);
    }
    Animate3d() {}
    void setup(AnimRepeat repeat, float seconds);
    void update() { animatorUp.update(1.0f / ofGetTargetFrameRate()); animatorDown.update(1.0f / ofGetTargetFrameRate());};
    bool isAnimating() { return animatorUp.isAnimating() || animatorDown.isAnimating(); }
    ofxAnimatableFloat& getUpAnimator() { return animatorUp; }
    ofxAnimatableFloat& getDownAnimator() { return animatorDown; }
    void setRotation(const ofVec3f& r) { currentRotation = r; }
protected:
    ofxAnimatableFloat animatorUp;
    ofxAnimatableFloat animatorDown;
    ofVec3f currentRotation;
};

class GameObject : public Animate3d {
public:
    GameObject(const ofRectangle& rectangleIn, objectTexture textureIn, int idIn, AnimRepeat repeat, float seconds, of3dPrimitive *parentIn) :Animate3d(repeat, seconds){
        setRectangle(rectangleIn);
        setup(seconds, repeat);
        texture = textureIn;
        parent = parentIn;
        id = idIn;
    }
    GameObject():Animate3d(){
        parent = nullptr;
        id = -1;
        running = false;
    }

    virtual ~GameObject() { }

    void setup(float seconds, AnimRepeat repeat= PLAY_ONCE) {
        Animate3d::setup(repeat, seconds);
        running = true;// start off running
    }
    virtual void setRectangle(const ofRectangle& rectangleIn) {
        rectangle = rectangleIn;
    }
    bool operator==(const int rhs) const {
        return id == rhs;
    }
    bool operator==(const ofRectangle& rhs) const {
        return rectangle == rhs;
    }

    bool isRunning() const { return running; }
    void stop() { running = false; }
    int getID() { return id; }
    Sound&getSound() { return sound; }

protected:
    ofRectangle rectangle;
    int id;
    Sound sound;
    objectTexture texture;
    bool running;
    of3dPrimitive *parent;
};

class SuperSphere : public GameObject, public ofSpherePrimitive {
public:
    SuperSphere(const ofRectangle& rectangle, objectTexture texture, int id, float seconds, of3dPrimitive *parent) :GameObject(rectangle, texture, id, PLAY_ONCE, seconds, parent) { setup(); }
    SuperSphere() :GameObject() { setup(); }

    void draw();
    void setup();
    void home() {
        setOrientation({ 0.f,0.f,0.f });
    }
    virtual void setRectangle(const ofRectangle& rectangleIn) {
        rectangle = rectangleIn;
        setPosition(rectangle.x, rectangle.y, 0.0f);
        setRadius(::getRadiusGlobal(rectangle.width, rectangle.height));
    }

};

class SuperCube : public GameObject, public ofBoxPrimitive {
public:
    SuperCube(const ofRectangle& rectangle, objectTexture texture, int id, float seconds, of3dPrimitive *parent) :GameObject(rectangle, texture, id, PLAY_ONCE, seconds, parent) { setup(); }
    void setup() {
    }
};

class SuperCylinder: public GameObject, public ofCylinderPrimitive {
public:
    SuperCylinder(const ofRectangle& rectangle, objectTexture texture, int id, float seconds, of3dPrimitive *parent) :GameObject(rectangle, texture, id, PLAY_ONCE, seconds, parent) { setup(); }

    void setup() {
        setHeight(rectangle.height / 2);
        setRadius(rectangle.width);
    }
};

class MainEye : public SuperSphere { // uses external texture
public:
    MainEye() :SuperSphere() { Animate3d::setup(LOOP_BACK_AND_FORTH, 0.0f); running = true; }
};

class EyeGameItem : public SuperSphere {
public:
    EyeGameItem(const ofRectangle& rectangle, objectTexture texture, int id, float seconds, of3dPrimitive *parent) :SuperSphere(rectangle, texture, id, seconds, parent) { setup(); }

    virtual  ~EyeGameItem() {  }

    void draw();

    void setup() {
        setRadius(getRadius()*2.0f);// why grow it again?
        getSound().setup(42.0f, 0.5f, 0.5f, 128.0f, 3);
    }
    void update() {
        Animate3d::update();
        if (!isAnimating()) {
            stop();
        }
        //sphere.rotateDeg(20.0f*sphere.getAnimator().val(), glm::vec3(0.0f, 1.0f, 0.0f));
        //int w = ofGetWidth();//bugbug p;ut in setup
        //int h = ofGetHeight();
        //sphere.rotateAroundDeg(15.0f*sphere.getAnimator().val(), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3());
        //ofNode node;
        //node.setPosition(0, 0, -rectangle.width / 2);
        //orbitDeg(5 * getUpAnimator().val(), ofRandom(360.0f), rectangle.width / 2 * 2, node);
        //setRadius(getRadius()*getDownAnimator().val());
    }
};

class CubeGameItem : public SuperCube {
public:
    CubeGameItem(const ofRectangle& rectangle, objectTexture texture, int id, float seconds, of3dPrimitive *parent) :SuperCube(rectangle, texture, id, seconds, parent) { setup(); }
    virtual  ~CubeGameItem() {  }
    void draw();
    void setup() {
        if (GameObject::parent) {
            glm::vec3 v3 = getPosition();
            v3.z = GameObject::parent->getZ();
            setPosition(v3);
        }
        getSound().setup(52.0f, 0.1f, 0.125f, 100.0f, 1);
    }
    void update() {
        Animate3d::update();
        if (!isAnimating()) {
            stop();
        }
        glm::vec3 newPos = getPosition();
        newPos.z = rectangle.width / 2 * getUpAnimator().val();  //movement*10;
        //newPos.x = movement;
        setPosition(newPos);
        setWidth(getWidth()*1.0f / getUpAnimator().val());
        setHeight(getHeight()*1.0f / getUpAnimator().val());

    }
};

class CylinderGameItem : public SuperCylinder {
public:
    CylinderGameItem(const ofRectangle& rect, objectTexture texture, int id, float seconds, of3dPrimitive *parent) :SuperCylinder(rect, texture, id, seconds, parent) { setup(CylinderGameItemTime); }
    virtual  ~CylinderGameItem() {  }
    void draw();

    void setup(float duration) {
        if (GameObject::parent) {
            glm::vec3 v3 = getPosition();
            v3.z = GameObject::parent->getZ();
            setPosition(v3);
        }
        getSound().setup(42.0f, 0.1f, 0.125f, 120.0f, 2);
    }
    void update() {
        Animate3d::update();
        if (!isAnimating()) {
            stop();
        }
        rotateDeg(20.0f*getUpAnimator().val(), 0.0f, 0.0f, 1.0f);
        glm::vec3 newPos = getPosition();
        newPos.z = rectangle.width / 2 * getUpAnimator().val();
        newPos.x = rectangle.width / 2 * getUpAnimator().val();
        if (newPos.z > rectangle.width / 2 * 3) {
            newPos.x = rectangle.width / 2 * getUpAnimator().val();
            newPos.z = rectangle.width / 2;
        }
        setPosition(newPos);
    }
};

class GameLevel {
public:
    enum Levels { NoGame = -1, Basic = 0, Medium = 1, Difficult = 2 };
    enum Durations { NoDuration = 0, BasicDuration = 60, MediumDuration = 60, DifficultDuration = 60 };

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
    bool addGameItem(LocationToActionMap* rect);
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
    MainEye mainEye;
    Textures rotatingEyesSkins;
    MainEye rotatingEye;
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
    std::list<std::shared_ptr<EyeGameItem>> gameEyes; // if you are in this list you have been found and not time out has occured bugbug add time out
    std::list<std::shared_ptr<CubeGameItem>> gameCubes;
    std::list<std::shared_ptr<CylinderGameItem>> gameDiscs;
    ofxAnimatableFloat blinker; // blink animation
};
