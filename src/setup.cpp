#include "ofApp.h"
#include "sound.h"

void Animate3d::setup(AnimRepeat repeat, float seconds){
    animatorUp.reset(0.0001f); // do no make 0, some divs will fault
    animatorUp.setDuration(seconds);
    animatorUp.setRepeatType(repeat);
    animatorUp.setCurve(LINEAR);
    if (seconds) {
        animatorUp.animateTo(1.0f);
    }

    animatorDown.reset(1.0f); 
    animatorDown.setDuration(seconds);
    animatorDown.setRepeatType(repeat);
    animatorDown.setCurve(LINEAR);
    if (seconds) {
        animatorDown.animateTo(0.0001f);
    }
}

void SuperSphere::setup() {
    if (GameObject::parent) {
        setParent(*GameObject::parent);
        lookAt(*GameObject::parent);  // do we want ths?
    }
    setResolution(27);
    setPosition(rectangle.x/2.0f, rectangle.y / 2.0f, 0.0f);
    setRadius(::getRadiusGlobal(rectangle.width, rectangle.height));

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
