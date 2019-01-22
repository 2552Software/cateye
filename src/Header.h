#pragma once

#include "ofApp.h"

const int frameRate = 15;
const float cameraWidth = 640;// 320; // the motion image from the camera
const float cameraHeight = 480;//240;
inline float scaleX() {
    return (float)ofGetWidth() / cameraWidth;
}
inline float scaleY() {
    return (float)ofGetHeight() / cameraHeight;
}
class TextTimer {
public:
    TextTimer(const std::string& textIn, unsigned int timeToRenderIn, unsigned int delay, int lineIn);
    void update();
    bool completed() const { return elapsed > timeDelay + lingerTime; }
    std::string& getPartialString();
    int getLine() { return line; }

private:
    unsigned int elapsed, timeDelay, lingerTime; // ticks (ie calls to update)
    std::string& getRawText() { return rawText; }
    std::string rawText; // raw text
    std::string finalText;
    int line; // user define value
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
    void draw(float w, float h, float z, bool drawcontours);
    ofxCvContourFinder contourFinder;
    ofxCvContourFinder contourDrawer;
    ofxCvGrayscaleImage grayImage, backgroundImage, grayDiff, grayDraw;

private:
    ofVideoGrabber video;
    ofxCvColorImage colorImg;
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
    void update();
    float getSeconds() { return seconds; }
    void setRotation(const ofVec3f& r) { currentRotation = r; }

    ofxAnimatableFloat animatorUp;
    ofxAnimatableFloat animatorDown;
    ofVec3f currentRotation;
    float seconds;
};

class GameObject : public Animate3d {
public:
    GameObject(float x, float y, float z, int idIn, AnimRepeat repeat, float seconds);
    GameObject();

    void setup(float xIn, float yIn, float zIn, float seconds, AnimRepeat repeat = PLAY_ONCE);
    bool operator==(const int rhs) const {
        return id == rhs;
    }
    bool isRunning() const { return running; }
    void stop() { running = false; }
    void start() { running = true; }
    int getID() { return id; }
    Sound&getSound() { return sound; }

protected:
    int id;
    Sound sound;
    bool running;
    float x, y, z;
};

class SuperSphere : public GameObject {
public:
    SuperSphere(float x, float y, float z, float r, int id=-1, float seconds=0.0f) :GameObject(x,y,z,id, PLAY_ONCE, seconds) { setup(r); }
    SuperSphere() :GameObject() { setup(0.0f); }

    void draw();
    void setup(float r);
    void update();
    void home() {
        sphere.setOrientation({ 0.f,0.f,0.f });
        sphere.panDeg(180); // like a FG kickers - laces out
    }
    ofSpherePrimitive sphere;
    ofxAnimatableFloat rotater;

};

class MainEye : public SuperSphere { // uses external texture
public:
    MainEye(float x=0.0f, float y = 0.0f, float z=0.0f, float r = 0.0f) :SuperSphere(x,y,z,r) {  }
};

class CrazyEye : public SuperSphere { // uses external texture
public:
    CrazyEye(float x = 0.0f, float y = 0.0f, float z = 0.0f, float r = 0.0f) :SuperSphere(x, y, z, r) {
    }
    void draw();
};

class EyeGameItem : public SuperSphere {
public:
    EyeGameItem(float x, float y, float z, float r, int id, float seconds) :
        SuperSphere(x,y,z,r,id, seconds) {}
};

class GameLevel {
public:
#define MAXLEVELS 4
    enum Levels { NoGame = 0, Basic = 1, Medium = 2, Difficult = 3 };

    GameLevel() : durations{120* frameRate, 10 * frameRate, 15 * frameRate, 20 * frameRate } // measured in number of calls to update, about 15/second if that is the frame rate
    { setup(NoGame);  }

    void setup(Levels level);
    void update() {
        ++gameLevelTime;
    }

    bool inGame() { return getLevel() != NoGame; }
    bool checkTimeOut();

    void  resetLevelTime() { gameLevelTime = 0; }
    Sound&getSound() { return sound; }

    Levels getLevel() { return level; }
    int timeLeft() { return durations[level] - gameLevelTime; }
    void next();
    void prev();
private:
    float gameLevelTime;
    Sound sound;
    Levels level;
    int durations[MAXLEVELS];
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
    void clear() {
        fullScreenText.clear();
    }
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
    bool animateString(TextTimer& text);
    int id;
    std::function<void(int, bool)> callback;
    void call(bool bInline) {
        if (callback) {
            callback(id, bInline);
        }
    }
};

class Blinker {
public:
    void setup();
    void update();
    void draw(float r);
    float val() { return blinker.val(); }
private:
    ofxAnimatableFloat blinker; // blink animation

};

class SoundPlayer : public ofSoundPlayer{
public:
    std::string name;
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
    void credits(bool signon = false);
    ContoursBuilder contours;
    float maxForTrigger;
    float r; 
    ofRectangle convert(const ofRectangle&rect) {
        return ofRectangle(rect.x*scaleX(), rect.y*scaleY(), rect.width*scaleX(), rect.height*scaleY());
    }
    void setSoundLoop(const std::string& name, bool b) {
        for (auto&a : mySounds) {
            if (a.name == name) {
                a.setLoop(b);
                break;
            }
        }
    }
    void playSound(const std::string& name) {
        for (auto&a : mySounds) {
            if (a.name == name) {
                a.play();
                break;
            }
        }
    }
    void playMoreRandomSound(const std::string& name) {
        for (auto&a : mySounds) {
            if (a.name == name) {
                a.setSpeed(ofRandom(0.5, 1.4));
                a.play();
                break;
            }
        }
    }
    void stopSound(const std::string& name) {
        for (auto&a : mySounds) {
            if (a.name == name) {
                a.stop();
                break;
            }
        }
    }

private:
    std::vector<SoundPlayer> mySounds;
    GameLevel current;// allocation not validated
    void removeGameItem(int id);
    bool addGameItem(LocationToActionMap* rect);
    TextEngine basicText;
    TextEngine fancyText;
    bool find(int id);
    void nextLevel();
    void setTriggerCount(float count=20.0f);
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
    CrazyEye rotatingEye;
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
    bool sendFireworks;
    ofVec3f currentLocation;
    ofVec3f currentRotation;
    Blinker blinker;
    typedef std::pair<float, float> Key;
    std::map<Key, float> mapCameraInX; // range to rotation
    std::map<Key, float> mapCameraInY;
    std::vector<std::map<std::pair<int, int>, LocationToActionMap>> aimationMaps; // map indexes, nullptr means no object found yet
    std::list<EyeGameItem> gameEyes; // if you are in this list you have been found and not time out has occured bugbug add time out

};
