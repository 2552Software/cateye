#include "ofApp.h"

GameItem::GameItem(const ofRectangle& rect, Eye eye, int a) {
    rectangle = rect;
    myeye = eye;
    box.setPosition(rect.x, rect.y + rect.height, 0.0f);
    box.setWidth(rect.width);
    box.setHeight(rect.height);
    sphere.setRadius(min(rect.height, rect.width) / 2);
    // use with squares etc sphere.set(rect.width, rect.height, 0.0f);
    sphere.setPosition(rect.x, rect.y, 0.0f);
    alpha = a;
    setup();
}



void GameItem::setup() {
    color.setColor(ofColor::white);
    color.setDuration(5.0f);
    color.setRepeatType(PLAY_ONCE);
    color.setCurve(EASE_IN_EASE_OUT);
    color.animateTo(ofColor::blue);
}

void Eyes::setup(AnimRepeat repeat, float seconds, const std::string& path, bool blink, float rotateIn) {
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
            add(dir.getName(i), dir.getPath(i), blink); //blink
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
    level = 0;
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

    mainEyes.setup(PLAY_ONCE, 1.0f, DATAPATH, true, 0.0f);
    if (!mainEyes.count()) {
        ofLogFatalError() << "eyes missing";
        ofExit(100);
    }

    std::string path = DATAPATH;
    path += "\\spirl";
    rotatingEyes.setup(LOOP_BACK_AND_FORTH_ONCE, 3.0f, path, false, 25.0f);
    if (!rotatingEyes.count()) {
        ofLogError() << "rotating eyes missing";
    }

    //path.setup();
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

    clear(); // go to a known state (call last like this as it may depend on othe settings)
    startPlaying();

}
