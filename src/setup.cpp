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
class internalEye {
public:
    void setup(AnimRepeat repeat, float seconds) {
        mainEyeAnimatorIndex.reset(0.0f);
        mainEyeAnimatorIndex.setDuration(seconds);
        mainEyeAnimatorIndex.setRepeatType(repeat);
        mainEyeAnimatorIndex.setCurve(LINEAR);
    }
    void update() {

    }
    void draw() {

    }
private:
    ofxAnimatableFloat mainEyeAnimatorIndex;
};
void ImageAnimator::setup() {

    font.load("alger.ttf", 100, true, true, true);
    font.setLineHeight(18.0f);
    font.setLetterSpacing(1.037);

    ofAddListener(rotatingEyeZ.animFinished, this, &ImageAnimator::spirlDone);
    ofAddListener(textFinished, this, &ImageAnimator::creditsDone);

    buildX();
    buildY();
    reset();

    mainEyeAnimatorIndex.reset(0.0f);
    mainEyeAnimatorIndex.setDuration(1.0f);
    mainEyeAnimatorIndex.setRepeatType(LOOP_BACK_AND_FORTH);
    mainEyeAnimatorIndex.setCurve(LINEAR);

    rotatingEyeAnimatorIndex.reset(0.0f);
    rotatingEyeAnimatorIndex.setDuration(1.0f);
    rotatingEyeAnimatorIndex.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    rotatingEyeAnimatorIndex.setCurve(LINEAR);
    
    path.setup();
    rotator.setup();
    credits(); // setup credits, shown at boot
    contours.setup();

    string path = DATAPATH;

    ofDirectory rollingEyesDir(path+ "\\spirl");
    rollingEyesDir.allowExt("png");
    rollingEyesDir.allowExt("jpg");
    size_t i = 0;
    if (rollingEyesDir.listDir() > 0) {
        for (; i < rollingEyesDir.size(); i++) {
            rollingEyes.push_back(SuperSphere());
            rollingEyes[rollingEyes.size() - 1].setup(rollingEyesDir.getName(i), "");
        }
    }
    else {
        ofLogError() << "rolling eyes missing"; // fatal
    }

    ofDirectory allEyesDir(path);
    allEyesDir.allowExt("png");
    allEyesDir.allowExt("jpg");
    i = 0;
    if (allEyesDir.listDir() > 0) {
        for (; i < allEyesDir.size(); i++) {
            add(allEyesDir.getPath(i), allEyesDir.getName(i));
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
