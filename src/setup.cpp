#include "ofApp.h"

void Animate3d::setup(AnimRepeat repeat, float seconds){
    animator.reset(0.0f);
    animator.setDuration(seconds);
    animator.setRepeatType(repeat);
    animator.setCurve(LINEAR);
}
void GameItem::set(Levels levelIn, float durationIn) {
    duration = durationIn;
    level = levelIn;
}
void SuperSphere::setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h) {
    setResolution(27);
    setRadius(::getRadiusGlobal(w, h));
    Animate3d::setup(repeat, seconds);
    setPosition(x, y, 0.0f);
}

void SuperCylinder::setup(AnimRepeat repeat, float seconds, float x, float y, int w, int h) {
    setHeight(h/25.0f);
    setRadius(w/25);
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
    resetLevelTime();
    running = true;// start off running
    set(level, duration);
}

void GameItem::setupHelper(of3dPrimitive* primitive, ofNode *parent) {
    if (parent) {
        primitive->setParent(*parent);
    }
    glm::vec3 v3 = primitive->getPosition();
    primitive->setPosition(v3.x, v3.y, getRadiusGlobal());
}

void CylinderGameItem::setup(ofNode *parent) {
    setupHelper(&cylinder, parent);
    cylinder.setup(PLAY_ONCE, 30.0f, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    cylinder.getAnimator().animateTo(1.0f);
}

void CubeGameItem::setup(ofNode *parent) {
    setupHelper(&cube, parent);
    cube.setup(PLAY_ONCE, 30.0f, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    cube.getAnimator().animateTo(1.0f);

}
void SphereGameItem::setup(ofNode *parent) {
    setupHelper(&sphere, parent);
    sphere.setup(PLAY_ONCE, 30.0f, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    sphere.getAnimator().animateTo(1.0f);


/*
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
       sphere.setParent(parent);
       sphere.setup(PLAY_ONCE, 1.0f, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
       duration = 30.0f;
    }
*/
}

void Textures::setup(const std::string& path) {

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

    ofAddListener(rotatingEye.getAnimator().animFinished, this, &Game::rotatingEyesDone);

    buildTable();
    buildX();
    buildY();

    mainEyesSkins.setup(EYES);
    rotatingEyesSkins.setup(SPIRALS);
    cubesSkins.setup(CUBES);
    spheresSkins.setup(SPHERES);
    musicNotesSkins.setup(MUSICNOTES);
    cylindersSkins.setup(CYLINDERS);
    
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
