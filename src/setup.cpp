#include "ofApp.h"

void Sound::setup(ofApp*pApp) {
    wavePhase = 0;
    pulsePhase = 0;

    // start the sound stream with a sample rate of 44100 Hz, and a buffer
    // size of 512 samples per audioOut() call
    ofSoundStreamSettings settings;
    settings.numOutputChannels = 2;
    settings.sampleRate = 44100;
    settings.bufferSize = 512;
    settings.numBuffers = 4;
    settings.setOutListener(pApp);
    soundStream.setup(settings);

    volume = 0.1f;
    frequencyTarget = 172.0f;
    frequency = 0.0f;
}
void SuperSphere::setup(const string&name) {
    eye.setup(name);
    setResolution(21);
    panDeg(180);
}
void TextTimer::setup() {
}

void Eye::setup(const string&texName) {
    if (ofLoadImage(*this, texName)) {
        ofLogNotice("Eye") << "loaded " << texName;
    }
    else {
        ofLogError("Eye") << "not loaded " << texName;
    }
}

GameItem::GameItem(const ofRectangle& rect, Eye eye, int levelIn) {
    rectangle = rect;
    level = levelIn;
    myeye = eye;
    setup();
}

void GameItem::setup() {
    frequency = 0.0f;

    animater.reset(0.0f);
    animater.setCurve(EASE_IN_EASE_OUT);
    animater.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);

    box.setPosition(rectangle.x, rectangle.y + rectangle.height, -2*rectangle.width);
    box.setWidth(rectangle.width);
    box.setHeight(rectangle.height);
    sphere.setRadius(min(rectangle.height, rectangle.width) / 2);
    // use with squares etc sphere.set(rect.width, rect.height, 0.0f);
    sphere.setPosition(rectangle.x, rectangle.y, 0.0f);

    if (level > 1) {
        animater.setDuration(20.0f); //bugbug menu
    }
    else {
        animater.setDuration(30.0f); //bugbug menu
    }
    animater.animateTo(1.0f);
}

void Eyes::setup(AnimRepeat repeat, float seconds, const std::string& path, float rotateIn) {
    rotate = rotateIn;

    getAnimator().reset(0.0f);
    getAnimator().setDuration(seconds);
    getAnimator().setRepeatType(repeat);
    getAnimator().setCurve(LINEAR);

    ofDirectory dir(path);
    dir.allowExt("png");
    dir.allowExt("jpg");
    size_t i = 0;
    if (dir.listDir() > 0) {
        selector.reset(0.0f);
        selector.setDuration(60.0f); // do not pick too often bugbug menu
        selector.setRepeatType(LOOP_BACK_AND_FORTH);
        selector.setCurve(LINEAR);
        selector.animateTo(dir.size()-1);
        for (; i < dir.size(); i++) {
            add(dir.getName(i), dir.getPath(i));
        }
    }
    else {
        ofLogError() << "eyes missing"; // fatal?
    }
}

void ContoursBuilder::setup() {
    vector<ofVideoDevice> devices = video.listDevices();
    for (auto& device : devices) {
        if (device.deviceName.find("facetime") == std::string::npos) {
            video.setDeviceID(device.id);
            break;
        }
    }
    video.setVerbose(true);
    video.setup(cameraWidth, cameraHeight);
    colorImg.allocate(cameraWidth, cameraHeight);
    grayImage.allocate(cameraWidth, cameraHeight);
    grayDiff.allocate(cameraWidth, cameraHeight);
}

void ImageAnimator::setup() {
    level = -1;
    // convert to screen size
    xFactor = ofGetScreenWidth() / cameraWidth;
    yFactor = ofGetScreenHeight() / cameraHeight;

    font.load("alger.ttf", 100, true, true, true);
    font.setLineHeight(18.0f);
    font.setLetterSpacing(1.037);

    ofAddListener(rotatingEyes.getAnimator().animFinished, this, &ImageAnimator::rotatingEyesDone);
    ofAddListener(textFinished, this, &ImageAnimator::creditsDone);

    buildTable();
    buildX();
    buildY();
    imagPath.setup();

    mainEyes.setup(PLAY_ONCE, 1.0f, DATAPATH, 0.0f);
    if (!mainEyes.count()) {
        ofLogFatalError() << "eyes missing";
        ofExit(100);
    }

    std::string path = DATAPATH;
    path += "\\spirl";
    rotatingEyes.setup(LOOP_BACK_AND_FORTH_ONCE, 3.0f, path, 25.0f);
    if (!rotatingEyes.count()) {
        ofLogError() << "rotating eyes missing";
    }

    path = DATAPATH;
    path += "\\cube\\cube.jpg";
    cube.setup(path);
    path = DATAPATH;
    path += "\\circle\\eye1.jpg";
    sphere.setup(path);


    //path.setup(); path not used this release, allows eye to move all over
    rotator.setup();
    contours.setup();

    ofDirectory allSounds(DATAPATH);
    allSounds.allowExt("wav");
    allSounds.allowExt("mp3");
    allSounds.listDir();
    for (size_t i = 0; i < allSounds.size(); i++) {
        ofSoundPlayer sound;
        sound.load(allSounds.getPath(i));
        mySounds.push_back(sound);
    }

    blinker.reset(0.0f);
    blinker.setCurve(EASE_IN_EASE_OUT);
    blinker.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    blinker.setDuration(1.0f);
    blinker.animateTo(1.0f);

    clear(); // go to a known state (call last like this as it may depend on othe settings)
    startPlaying();

}
