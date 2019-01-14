#pragma once

#include "ofApp.h"

const float cameraWidth = 640;// 320; // the motion image from the camera
const float cameraHeight = 480;//240;
const float windowWidth = 1024;
const float windowHeight = 768;
// convert to screen size
const float xFactor = windowWidth / cameraWidth;
const float yFactor = windowHeight / cameraHeight;
inline ofRectangle convert(const ofRectangle&rect) {
    return ofRectangle(rect.x*xFactor, rect.y*yFactor, rect.width*xFactor, rect.height*yFactor);
}
inline float getRadiusGlobal(int w= windowWidth, int h= windowHeight) {
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
    #define MaxSound 4
public:
    Sound(int sequencer = 0)
        : pitches{ 36.0f, 40.0f, 50.0f, 72.0f },
        amps{0.25f, 0.25f, 0.5f, 1.0f},
        triggers{ 1.0f, 1.0f, 1.0f, 1.0f },
        tempos{ 90.0f, 100.0f, 108.0f, 115.0f }
    { setup(sequencer); }

    void setup(int sequencer = 0);
    bool sendSound() { return soundSet; }
    void setSound(bool b) { soundSet = b; }
    int getSequence() const { return sequencer; }
    float getAmp() const { return amps[getSequence()]; }
    float getPitch()const { return pitches[getSequence()]; }
    float getTrigger() const { return triggers[getSequence()]; }
    float getTempo() const { return tempos[getSequence()]; }

private:
    int sequencer;
    float pitches[MaxSound];
    float amps[MaxSound];
    float triggers[MaxSound];
    float tempos[MaxSound];
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
    float getSeconds() { return seconds; }
    bool isAnimating() { return animatorUp.isAnimating() || animatorDown.isAnimating(); }
    ofxAnimatableFloat& getUpAnimator() { return animatorUp; }
    ofxAnimatableFloat& getDownAnimator() { return animatorDown; }
    void setRotation(const ofVec3f& r) { currentRotation = r; }
protected:
    ofxAnimatableFloat animatorUp;
    ofxAnimatableFloat animatorDown;
    ofVec3f currentRotation;
    float seconds;
};

class GameObject : public Animate3d {
public:
    GameObject(float x, float y, float z, int idIn, AnimRepeat repeat, float seconds, of3dPrimitive *parentIn) :Animate3d(repeat, seconds){
        setup(x, y, z, seconds, repeat);
        parent = parentIn;
        id = idIn;
    }
    GameObject():Animate3d(){
        parent = nullptr;
        id = -1;
        running = false;
        x = y = z = 0;
    }

    virtual ~GameObject() { }

    void setup(float xIn, float yIn, float zIn, float seconds, AnimRepeat repeat= PLAY_ONCE) {
        x = xIn;
        y = yIn;
        z = zIn;
        Animate3d::setup(repeat, seconds);
        running = true;// start off running
    }
    bool operator==(const int rhs) const {
        return id == rhs;
    }
    bool isRunning() const { return running; }
    void stop() { running = false; }
    int getID() { return id; }
    Sound&getSound() { return sound; }

protected:
    int id;
    Sound sound;
    bool running;
    of3dPrimitive *parent;
    float x, y, z;
};

class SuperSphere : public GameObject, public ofSpherePrimitive {
public:
    SuperSphere(float x, float y, float z, float r, int id=-1, float seconds=0.0f, of3dPrimitive *parent=nullptr) :GameObject(x,y,z,id, PLAY_ONCE, seconds, parent) { setup(r); }
    SuperSphere() :GameObject() { setup(0.0f); }

    void draw();
    void setup(float r);
    void update();
    void home() {
        setOrientation({ 0.f,0.f,0.f });
        panDeg(180); // like a FG kickers - laces out
    }

};

class MainEye : public SuperSphere { // uses external texture
public:
    MainEye(float x=0.0f, float y = 0.0f, float z=0.0f, float r = 0.0f) :SuperSphere(x,y,z,r) { Animate3d::setup(LOOP_BACK_AND_FORTH, 0.0f); running = true; }
};

class EyeGameItem : public SuperSphere {
public:
    EyeGameItem(float x, float y, float z, float r, int id, float seconds, of3dPrimitive *parent) :
        SuperSphere(x,y,z,r,id, seconds, parent) {}
};

class GameLevel {
public:
#define MAXLEVELS 4
    enum Levels { NoGame = 0, Basic = 1, Medium = 2, Difficult = 3 };
    
    GameLevel() : durations{10.0f, 60.0f, 60.0f, 60.0f }
    { setup(NoGame);  }

    bool inGame() { return getLevel() != NoGame; }
    void advance();

    void  resetLevelTime() { gameLevelTime = ofGetElapsedTimef(); }
    Sound&getSound() { return sound; }

    Levels getLevel() { return level; }
    float timeLeft() { return durations[level] - getLevelDuration(); }
    float getLevelDuration() { return ofGetElapsedTimef() - gameLevelTime; }
    void setup(Levels level);
    void next();
private:
    float gameLevelTime;
    Sound sound;
    Levels level;
    float durations[MAXLEVELS];
};



// map location to interesting things
class LocationToActionMap : public ofRectangle {
public:
    LocationToActionMap() {
        c = 0; 
        level = GameLevel::NoGame;
    }
    void draw();
    int c; // countid
    GameLevel::Levels getLevel() { return  level; }
    void set(GameLevel::Levels levelIn) { level = levelIn; }
private:
    GameLevel::Levels level;
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
    bool inGame() { return current.inGame(); }
    float w, h;
    ContoursBuilder contours;
    float maxForTrigger;

private:
    GameLevel current;// allocation not validated
    void removeGameItem(int id);
    bool addGameItem(LocationToActionMap* rect);
    TextEngine basicText;
    TextEngine fancyText;
    bool find(int id);
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
    std::vector<std::map<std::pair<int, int>, LocationToActionMap>> aimationMaps; // map indexes, nullptr means no object found yet
    std::list<EyeGameItem> gameEyes; // if you are in this list you have been found and not time out has occured bugbug add time out
    ofxAnimatableFloat blinker; // blink animation
};
