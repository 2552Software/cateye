#include "ofApp.h"
#include "sound.h"


GameObject::GameObject(float x, float y, float z, int idIn, AnimRepeat repeat, float seconds) :Animate3d(repeat, seconds) {
    setup(x, y, z, seconds, repeat);
    id = idIn;
    stop();
}
GameObject::GameObject() :Animate3d() {
    id = -1;
    stop();
    x = y = z = 0;
}

void Animate3d::setup(AnimRepeat repeat, float secondsIn){

    seconds = secondsIn;
    animatorUp.reset(0.0001f); // do no make 0, some divs will fault
    animatorUp.setDuration(seconds);
    animatorUp.setRepeatType(repeat);
    if (seconds) {
        animatorUp.animateTo(1.0f);
    }

    animatorDown.reset(1.0f); 
    animatorDown.setDuration(seconds);
    animatorDown.setRepeatType(repeat);
    if (seconds) {
        animatorDown.animateTo(0.0001f);
    }
}
void GameObject::setup(float xIn, float yIn, float zIn, float seconds, AnimRepeat repeat) {
    x = xIn;
    y = yIn;
    z = zIn;
    Animate3d::setup(repeat, seconds);
    stop();
}

void SuperSphere::setup(float r) {
    rotater.reset(1); // do no make 0, some divs will fault
    rotater.setDuration(5.0f);
    rotater.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    sphere.setResolution(27);
    sphere.setPosition(x, y, z);
    sphere.setRadius(r);
    home();
}

void GameLevel::setup(Levels levelIn) {
    level = levelIn;
    sound.setup(level);
    resetLevelTime();
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

void Sound::setup(int sequencerIn) {
    if (sequencerIn < MaxSound) {
        sequencer = sequencerIn;
    }
    else {
        sequencer = 0;
    }
    setSound(true);
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
void Blinker::setup() {
    blinker.reset(0.0f);
    blinker.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    blinker.setDuration(1.0f);
}
void Game::setup() { 
    blinker.setup();
    setTriggerCount();
    setShapeMinSize();
    setSquareCount();

    basicText.setup(12);
    fancyText.setup(15);

    std::function<void(int, bool)> f = std::bind(&Game::textDone, this, std::placeholders::_1, std::placeholders::_2);
    fancyText.bind(f);

    ofAddListener(rotatingEye.animatorUp.animFinished, this, &Game::rotatingEyesDone);

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

    clear(); // go to a known state (call last like this as it may depend on othe settings)
    //bugbug credits(true);
    startPlaying();

    ofLogNotice("ImageAnimator::setup") << "finshed";
}
