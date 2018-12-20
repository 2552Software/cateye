#include "ofApp.h"

void SuperSphere::setup(const string&name) {
    eye.setup(name);
    r = getRadius();
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

GameItem::GameItem(const ofRectangle& rect, Eye eye, Levels levelIn, int idIn) {
    id = idIn;
    rectangle = rect;
    level = levelIn;
    myeye = eye;
    setup();
}

void GameItem::setup() {

    animater.reset(0.0f);
    animater.setCurve(EASE_IN_EASE_OUT);
    animater.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    float duration=20.0f;

    if (level == EndGame) {
        duration = 60.0f; //this is the big prize, enjoy it
    }
    else if (level == Difficult) {
        cylinder.setRadius(min(rectangle.height, rectangle.width) / 2);
        cylinder.setHeight(10); // thin
        // use with squares etc sphere.set(rect.width, rect.height, 0.0f);
        cylinder.setPosition(rectangle.x, rectangle.y, 0.0f);
        duration = 60.0f; 
    }
    else if (level == Medium) {
        box.setPosition(rectangle.x, rectangle.y + rectangle.height, -2 * rectangle.width);
        box.setWidth(rectangle.width);
        box.setHeight(rectangle.height);
        duration = 20.0f;
    }
    else if (level == Basic) {
        sphere.setRadius(min(rectangle.height, rectangle.width) / 2);
        // use with squares etc sphere.set(rect.width, rect.height, 0.0f);
        sphere.setPosition(rectangle.x, rectangle.y, 0.0f);
        duration = 30.0f;
    }
    animater.setDuration(duration);
    animater.animateTo(1.0f);
}

void Eyes::setup(AnimRepeat repeat, float seconds, const std::string& path) {

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

void ImageAnimator::setup() {

    level = NoGame;
    gameStartTime = 0.0f; // zero time
    // convert to window size from camera sized
    xFactor = w / cameraWidth;
    yFactor = h / cameraHeight;

    font.load("DejaVuSans.ttf", 15, false, false, true);

    ofAddListener(rotatingEyes.getAnimator().animFinished, this, &ImageAnimator::rotatingEyesDone);
    ofAddListener(textFinished, this, &ImageAnimator::creditsDone);

    buildTable();
    buildX();
    buildY();

    mainEyes.setup(PLAY_ONCE, 1.0f, EYES);
    if (!mainEyes.count()) {
        ofExit(100);
    }
    
    rotatingEyes.setup(LOOP_BACK_AND_FORTH_ONCE, 3.0f, SPIRALS);
    cubes.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, CUBES);
    spheres.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, SPHERES);
    musicNotes.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, MUSICNOTES);
    cylinders.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, CYLINDERS);

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
    startPlaying();

    ofLogNotice("ImageAnimator::setup") << "finshed";
}
