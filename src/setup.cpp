#include "ofApp.h"


void Eyes::setup(AnimRepeat repeat, float seconds, const std::string& path, bool blink, float rotateIn) {
    rotate = rotateIn;
    animator.reset(0.0f);
    animator.setDuration(seconds);
    animator.setRepeatType(repeat);
    animator.setCurve(LINEAR);

    ofDirectory dir(path);
    dir.allowExt("png");
    dir.allowExt("jpg");
    size_t i = 0;
    if (dir.listDir() > 0) {
        for (; i < dir.size(); i++) {
            add(dir.getName(i), dir.getPath(i), blink);
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
    video.setup(imgWidth, imgHeight);
    colorImg.allocate(imgWidth, imgHeight);
    grayImage.allocate(imgWidth, imgHeight);
    grayDiff.allocate(imgWidth, imgHeight);
}
void ImageAnimator::setup() {
    reset(); // go to a known state

    font.load("alger.ttf", 100, true, true, true);
    font.setLineHeight(18.0f);
    font.setLetterSpacing(1.037);

    ofAddListener(rotatingEyes.eyesZ.animFinished, this, &ImageAnimator::rotatingEyesDone);
    ofAddListener(textFinished, this, &ImageAnimator::creditsDone);

    buildX();
    buildY();
    imagPath.setup();

    mainEyes.setup(LOOP_BACK_AND_FORTH, 1.0f, DATAPATH, true, 0.0f);
    if (!mainEyes.count()) {
        ofLogFatalError() << "eyes missing";
        ofExit(100);
    }

    std::string path = DATAPATH;
    path += "\\spirl";
    rotatingEyes.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, path, false, 20.0f);
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

    startPlaying();

}
