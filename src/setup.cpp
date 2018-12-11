#include "ofApp.h"

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

    font.load("alger.ttf", 100, true, true, true);
    font.setLineHeight(18.0f);
    font.setLetterSpacing(1.037);

    spirl.load("runtime\\spirl\\s1.png"); //bugbug use the multi load tech for main eye here
    sphere4Spirl.panDeg(180);
    sphere4Spirl.setResolution(21);
    ofAddListener(rotatingEyeZ.animFinished, this, &ImageAnimator::spirlDone);
    ofAddListener(textFinished, this, &ImageAnimator::creditsDone);

    buildX();
    buildY();
    reset();

    animatorIndex.reset(0.0f);
    animatorIndex.setDuration(1.0f);
    animatorIndex.setRepeatType(LOOP_BACK_AND_FORTH);
    animatorIndex.setCurve(LINEAR);

    path.setup();
    rotator.setup();
    credits(); // setup credits, shown at boot
    contours.setup();

    string path = DATAPATH;
    ofDirectory allEyes(path);
    allEyes.allowExt("png");
    allEyes.allowExt("jpg");
    size_t i = 0;
    if (allEyes.listDir() > 0) {
        for (; i < allEyes.size(); i++) {
            add(allEyes.getPath(i), allEyes.getName(i));
        }
    }
    else {
        ofLogFatalError() << "eyes missing";
        ofExit(100);
    }
    ofDirectory allSounds(path);
    allSounds.allowExt("wav");
    allSounds.allowExt("mp3");
    allSounds.listDir();
    for (i = 0; i < allSounds.size(); i++) {
        ofSoundPlayer sound;
        sound.load(allSounds.getPath(i));
        mySounds.push_back(sound);
    }

    startPlaying();

}
