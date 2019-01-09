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

template <class T>
class GameObject : public T, public Animate3d {
public:
    GameObject(const ofRectangle& rectangleIn, objectTexture textureIn, int idIn, of3dPrimitive *parentIn) {
        rectangle = rectangleIn;
        setPosition(rectangle.x, rectangle.y, 0.0f);
        texture = textureIn;
        parent = parentIn;
        if (parent) {
            obj.setParent(*parent);
        }
        id = idIn;
    }
    GameObject() {
        parent = nullptr;
        id = -1;
    }

    virtual ~GameObject() { }

    virtual void setup(float seconds) {
        if (parent) {
            obj.lookAt(*parent);  // do we want ths?
        }
        Animate3d::setup(PLAY_ONCE, seconds);
        sound.setup();
        running = true;// start off running
    }
    void setup(const ofRectangle& rectangleIn, AnimRepeat repeat) {
        rectangle = rectangleIn;
        Animate3d::setup(repeat, 0);
        sound.setup();
        running = true;// start off running
    }

    virtual void draw() {
        ofDrawRectangle(rectangle);
        texture.start();
        obj.draw();
        texture.stop();
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
    T obj;
};

class SuperSphere : public GameObject<ofSpherePrimitive> {
public:
    SuperSphere(const ofRectangle& rectangle, objectTexture texture, int id, of3dPrimitive *parent) :GameObject(rectangle, texture, id, parent) { setup(); }
    SuperSphere() :GameObject() {}

    void setup() {
        setResolution(27);
        setRadius(::getRadiusGlobal(rectangle.width, rectangle.height));
    }
    void setup(const ofRectangle& rectangle, AnimRepeat repeat) {
        GameObject::setup(rectangle, repeat);
        setup();
    }

    void home() {
        setOrientation({ 0.f,0.f,0.f });
    }

    void draw() {
        if (getRadius() > 0) {
            rotateDeg(currentRotation.x, 1.0f, 0.0f, 0.0f);
            rotateDeg(currentRotation.y, 0.0f, 1.0f, 0.0f);
            //drawWireframe();
            ofSpherePrimitive::draw();
            home();
            panDeg(180); // like a FG kickers - laces out
        }
    }
};

class SuperCube : public GameObject<ofBoxPrimitive> {
public:
    SuperCube(const ofRectangle& rectangle, objectTexture texture, int id, of3dPrimitive *parent) :GameObject(rectangle, texture, id, parent) {}

};

class SuperCylinder: public GameObject<ofCylinderPrimitive> {
public:
    SuperCylinder(const ofRectangle& rectangle, objectTexture texture, int id, of3dPrimitive *parent) :GameObject(rectangle, texture, id, parent) {}

    void setup(float seconds) {
        setHeight(rectangle.height / 2);
        setRadius(rectangle.width);
    }
};

const unsigned SphereGameItemTime = 10.0f;
class EyeGameItem : public SuperSphere {
public:
    EyeGameItem(const ofRectangle& rect, objectTexture texture, int id, of3dPrimitive *parent) :SuperSphere(rect, texture, id, parent) { setup(SphereGameItemTime); }
    virtual  ~EyeGameItem() {  }

    void setup(float duration) {
        GameObject::setup(duration);
        obj.setRadius(obj.getRadius()*2.0f);// why grow it again?
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
        ofNode node;
        node.setPosition(0, 0, -rectangle.width / 2);
        obj.orbitDeg(5 * getUpAnimator().val(), ofRandom(360.0f), rectangle.width / 2 * 2, node);
        obj.setRadius(obj.getRadius()*getDownAnimator().val());
    }
};

const unsigned CubeGameItemTime = 1.0f;
class CubeGameItem : public SuperCube {
public:
    CubeGameItem(const ofRectangle& rect, objectTexture texture, int id, of3dPrimitive *parent) :SuperCube(rect, texture, id, parent) { setup(CubeGameItemTime); }
    virtual  ~CubeGameItem() {  }
    void setup(float duration) {
        GameObject::setup(duration);
        if (parent) {
            glm::vec3 v3 = obj.getPosition();
            v3.z = parent->getZ();
            obj.setPosition(v3);
        }
        getSound().setup(52.0f, 0.1f, 0.125f, 100.0f, 1);
    }
    void update() {
        Animate3d::update();
        if (!isAnimating()) {
            stop();
        }
        glm::vec3 newPos = obj.getPosition();
        newPos.z = rectangle.width / 2 * getUpAnimator().val();  //movement*10;
        //newPos.x = movement;
        obj.setPosition(newPos);
        obj.setWidth(obj.getWidth()*1.0f / getUpAnimator().val());
        obj.setHeight(obj.getHeight()*1.0f / getUpAnimator().val());

    }
};

const unsigned CylinderGameItemTime = 10.0f;
class CylinderGameItem : public SuperCylinder {
public:
    CylinderGameItem(const ofRectangle& rect, objectTexture texture, int id, of3dPrimitive *parent) :SuperCylinder(rect, texture, id, parent) { setup(CylinderGameItemTime); }
    virtual  ~CylinderGameItem() {  }

    void setup(float duration) {
        GameObject::setup(duration);
        if (parent) {
            glm::vec3 v3 = obj.getPosition();
            v3.z = parent->getZ();
            obj.setPosition(v3);
        }
        getSound().setup(42.0f, 0.1f, 0.125f, 120.0f, 2);
    }
    void update() {
        Animate3d::update();
        if (!isAnimating()) {
            stop();
        }
        obj.rotateDeg(20.0f*getUpAnimator().val(), 0.0f, 0.0f, 1.0f);
        glm::vec3 newPos = obj.getPosition();
        newPos.z = rectangle.width / 2 * getUpAnimator().val();
        newPos.x = rectangle.width / 2 * getUpAnimator().val();
        if (newPos.z > rectangle.width / 2 * 3) {
            newPos.x = rectangle.width / 2 * getUpAnimator().val();
            newPos.z = rectangle.width / 2;
        }
        obj.setPosition(newPos);
    }
};

class GameLevel {
public:
    enum Levels { NoGame = -1, Basic = 0, Medium = 1, Difficult = 2 };
    enum Durations { NoDuration = 0, BasicDuration = 10, MediumDuration = 10, DifficultDuration = 10 };

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
    void pushSphere(const ofRectangle&rect, int id);
    void pushCube(const ofRectangle&rect, int id);
    void pushCylinder(const ofRectangle&rect, int id);
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
    std::list<std::shared_ptr<EyeGameItem>> gameEyes; // if you are in this list you have been found and not time out has occured bugbug add time out
    ofxAnimatableFloat blinker; // blink animation
};
