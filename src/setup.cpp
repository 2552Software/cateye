#include "ofApp.h"
#include "sound.h"

void Animate3d::setup(AnimRepeat repeat, float seconds, bool start){
    animatorUp.reset(0.0001f); // do no make 0, some divs will fault
    animatorUp.setDuration(seconds);
    animatorUp.setRepeatType(repeat);
    animatorUp.setCurve(LINEAR);
    if(start){
        animatorUp.animateTo(1.0f);
    }

    animatorDown.reset(1.0f); 
    animatorDown.setDuration(seconds);
    animatorDown.setRepeatType(repeat);
    animatorDown.setCurve(LINEAR);
    if (start) {
        animatorDown.animateTo(0.0f);
    }
    
}
void GameItem::set(Levels levelIn, float durationIn) {
    parent = nullptr;
    id = -1; // no id by default
    running = true;// start off running
    duration = durationIn;
    level = levelIn;
    resetLevelTime();
    pitch = 36.0f;
    amp = 0.0001f; 
    trigger = 1.0f;
    sequencer = 0;
}
void SuperSphere::setup(AnimRepeat repeat, float seconds, bool start, float x, float y, int w, int h) {
    setResolution(27);
    setRadius(::getRadiusGlobal(w, h));
    Animate3d::setup(repeat, seconds, start);
    setPosition(x, y, 0.0f);
}

void SuperCylinder::setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h) {
    setHeight(h/2);
    setRadius(w);
    setPosition(x, y, 0.0f);
    Animate3d::setup(repeat, seconds);
}

void SuperCube::setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h) {
    set(w, h, w);
    setPosition(x, y, 0.0f);
    Animate3d::setup(repeat, seconds);
}

void TextTimer::setup() {
}

void objectTexture::setup(const string&texName) {
    if (ofLoadImage(*this, texName)) {
        ofLogNotice("Eye") << "loaded " << texName;
    }
    else {
        ofLogError("Eye") << "not loaded " << texName;
    }
}

GameItem::GameItem(const ofRectangle& rect, objectTexture textureIn, int idIn, Levels level, float duration) {
    id = idIn;
    rectangle = rect;
    texture = textureIn;
    set(level, duration);
}

void GameItem::setupHelper(of3dPrimitive* primitive, SuperSphere* parentIn) {
    parent = parentIn; // ok if null
    if (parent) {
        primitive->setParent(*parent);
        glm::vec3 v3 = primitive->getPosition();
        v3.z = parent->getRadius();
        primitive->setPosition(v3);
    }
}

void CylinderGameItem::setup(SuperSphere *parent) {
    setupHelper(&cylinder, parent);
    cylinder.setup(PLAY_ONCE, duration, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    trigger = 0.125f;
    pitch = 52.0f;
    amp = 0.1f;
    sequencer = 1;
}

MusicItem::~MusicItem() {  
}
void MusicItem::setup(SuperSphere* parent, float pitchIn, float trigger, float ampIn) {
    setupHelper(&cylinder, parent);
    cylinder.setup(PLAY_ONCE, duration, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    cylinder.setHeight(rectangle.height);
    cylinder.setRadius(rectangle.width);
    cylinder.setPosition(rectangle.x, rectangle.y, 0.0f);
    cylinder.Animate3d::setup(PLAY_ONCE, duration);
    pitch = pitchIn;
    trig = trigger;
    amp = ampIn;
}

void CubeGameItem::setup(SuperSphere *parent) {
    setupHelper(&cube, parent);
    cube.setup(PLAY_ONCE, duration, rectangle.x, rectangle.y, rectangle.width, rectangle.height); // make bigger as we are zooming out
    trigger = 0.25f;
    pitch = 72.0f;
    amp = 1.00f;
    sequencer = 2;

}
void EyeGameItem::setup(SuperSphere *parent) {
    setupHelper(&sphere, parent);
    sphere.setup(PLAY_ONCE, duration, true, rectangle.x, rectangle.y, rectangle.width, rectangle.height); // make bigger as they will be zomed backwards
    sphere.setRadius(sphere.getRadius()*2.0f);
    if (parent) {
        sphere.lookAt(*parent);
    }
    trigger = 0.5f;
    pitch = 42.0f;
    amp = 0.001f;
    sequencer = 3;
}

void Textures::setup(const std::string& path, float duration) {

    ofDirectory dir(path);
    dir.allowExt("png");
    dir.allowExt("jpg");
    size_t i = 0;
    if (dir.listDir() > 0) {
        selector.reset(0.0f);
        selector.setDuration(duration); 
        selector.setRepeatType(LOOP);
        selector.setCurve(LINEAR);
        selector.animateTo(dir.size());
        for (; i < dir.size(); i++) {
            add(dir.getName(i), dir.getPath(i));
        }
    }
    else {
        ofLogError() << path << " missing"; // fatal?
    }
}

void ContoursBuilder::setup() {
    vector<ofVideoDevice> devices = video.listDevices();
    bool found = false;
    for (auto& device : devices) {
        ofLogVerbose("ContoursBuilder::setup") << device.deviceName << " " << device.id;
        if (device.deviceName.find("facetime") == std::string::npos) { // if its not face time use it
            video.setDeviceID(device.id);
            ofLogNotice("ContoursBuilder::setup found ") << device.deviceName;
            found = true;
            break;
        }
    }
    if (!found) {
        ofLogFatalError("ContoursBuilder::setup no device found");
        ofExit(1);
    }
    // only when degbugging video.setVerbose(true);
    video.setPixelFormat(OF_PIXELS_RGB);
    video.setDesiredFrameRate(15);
    if (!video.setup(cameraWidth, cameraHeight, false)) {
        ofLogFatalError("ContoursBuilder::setup video.setup failed");
        ofExit(1);
    }
    colorImg.allocate(cameraWidth, cameraHeight);
    grayImage.allocate(cameraWidth, cameraHeight);
    grayDiff.allocate(cameraWidth, cameraHeight);
    ofLogNotice("ContoursBuilder::setup over ");
}

void TextEngine::setup(int fontsize) {
    font.load("DejaVuSans.ttf", fontsize, false, false, true);
}
void Game::setup() { 

    setTriggerCount();
    setShapeMinSize();
    setSquareCount();

    current = std::make_shared<GameItem>();

    // convert to window size from camera sized
    xFactor = w / cameraWidth;
    yFactor = h / cameraHeight;

    basicText.setup(12);
    fancyText.setup(15);

    std::function<void(int, bool)> f = std::bind(&Game::textDone, this, std::placeholders::_1, std::placeholders::_2);
    fancyText.bind(f);

    ofAddListener(rotatingEye.getUpAnimator().animFinished, this, &Game::rotatingEyesDone);

    buildTable();
    buildX();
    buildY();

    mainEyesSkins.setup(EYES, 30.0f);
    rotatingEyesSkins.setup(SPIRALS, 10.0f);
    cubesSkins.setup(CUBES, 10.0f);
    spheresSkins.setup(SPHERES, 20.0f);
    musicNotesSkins.setup(MUSICNOTES, 20.0f);
    cylindersSkins.setup(CYLINDERS, 3.0f);
    
    contours.setup();

    //ofDirectory allSounds(SOUNDS);
    //allSounds.allowExt("wav");
   // allSounds.allowExt("mp3");
   // allSounds.listDir();
    //for (size_t i = 0; i < allSounds.size(); i++) {
      //  ofSoundPlayer sound;
       //bugbug not working on pi but replace with the cools stguf any sound.load(allSounds.getPath(i));
        //mySounds.push_back(sound);
   // }

    blinker.reset(0.0f);
    blinker.setCurve(EASE_IN_EASE_OUT);
    blinker.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    blinker.setDuration(1.0f);
    blinker.animateTo(1.0f);

    clear(); // go to a known state (call last like this as it may depend on othe settings)
    //bugbug credits(true);
    startPlaying();

    ofLogNotice("ImageAnimator::setup") << "finshed";
}
