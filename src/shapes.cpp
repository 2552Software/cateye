#include "ofApp.h"
void ImageAnimator::buildTable() {
    if (squareCount) {
        cameraMapping.clear();
        // all based on camera size and just grid out the screen 10x10 or what ever size we want
        float w = cameraWidth / squareCount; // menu bugbug
        float h = cameraHeight / squareCount;
        for (float x = w; x < cameraWidth - w; x += w) { // keep off the edges -- camera cannot always pick those up
            for (float y = h; y < cameraHeight - h; y += h) {
                // roate the x  to reflect viewer vs camera
                cameraMapping.insert(std::make_pair(std::make_pair(x, y), GameItem(ofRectangle(x, y, w, h)))); // build a default table
            }
        }
    }
}

void GameItem::setup() {
    rotateX = 0.0f;
}
void GameItem::set(int a) {
    action = a;
}

void GameItem::set(const ofRectangle& r) {
    rectangle = r;
}


void ImageAnimator::buildX() {
    float percent = 0.0f;// location as a percent of screen size
    float r = 30.0f; // rotation
    float incPercent = 5.0f;
    float incRotaion = ((r * 2) / (100.0f / incPercent - 1));
    for (int i = 1; percent < 100.0f; ++i, percent += incPercent, r -= incRotaion) {
        mapCameraInX.insert(std::make_pair(std::make_pair(percent, percent + incPercent), r));
    }
}
void ImageAnimator::buildY() {
    float percent = 0.0f;// location as a percent of screen size
    float r = -20.0f; // rotation
    float incPercent = 5.0f;
    float incRotaion = ((r * 2) / (100.0f / incPercent - 1));
    for (int i = 1; percent < 100.0f; ++i, percent += incPercent, r -= incRotaion) {
        mapCameraInY.insert(std::make_pair(std::make_pair(percent, percent + incPercent), r));
    }
}

TextTimer::TextTimer(const std::string& textIn, float timeToRenderIn, float delay, float lineIn) {
    line = lineIn;
    timeDelay = 0;
    done = false;
    text = textIn;
    timeSet = true;
    timeToRender = timeToRenderIn;
    timeDelay = delay;
    timeBegan = (int)ofGetElapsedTimeMillis();
    holdTextTime = 35.0f;
}

void ImageAnimator::circle() {
    ofxAnimatableOfPoint point;
    point.setPosition(currentLocation);
    point.setDuration(1.20f);
    point.animateTo(ofVec3f(1000, 1000, 10));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(1000, 2000, 200));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(2000, 2000, -200));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(1000, 1000, 3000));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(00, 00));
    imagPath.addTransition(point);
}

void TextTimer::setup() {
    
}
bool TextTimer::getString(std::string& output) {
    output.clear();
    int elapsedSeconds = ((int)ofGetElapsedTimeMillis() - timeBegan); //  20 seconds passed for example timeDelay
    if (timeDelay) {
        if (elapsedSeconds < timeDelay) {
            return false;
        }
        else {
            timeBegan = (int)ofGetElapsedTimeMillis(); // here we go
            timeDelay = 0.0f; // needs to be rest if used again
            return false; // get it next time
        }
    }
    if (!elapsedSeconds) {
        return false;
    }
    if (text.size() == 0 || timeToRender <= 0) {
        return false;
    }
    if (!done) {
        float factor = ((float)(elapsedSeconds) / (float)timeToRender);  // ratio of seconds that passed to our full range of time, say 20% or 0.2

        int n = (int)(factor * text.length());
        int max = min(n, (int)text.length());

        if (!n) {
            max = 1;
        }
        if (n >= (int)text.length()) {
            done = true;
            lasting.setColor(ofColor::white);
            lasting.setDuration(1.0f);
            lasting.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
            lasting.setCurve(EASE_IN_EASE_OUT);
            lasting.animateTo(ofColor::orangeRed);
        }
        output = text.substr(0, min(n, max));
        if (output.size() > 0) {
            return true;
        }
    }
    return false;
}

void ImageAnimator::draw(const std::string& s, float x, float y) {
    font.drawStringAsShapes(s, x, y);
}
void ImageAnimator::credits(bool signon) {
    /** bugbug removed */
    creditsText.push_back(TextTimer("fast for testing", 1000.0f, 0.0f, 0.0f));
    return;

    creditsText.clear();

    if (signon || (int)ofRandom(0, 10) > 2) {
        creditsText.push_back(TextTimer("Tom And Mark", 1000.0f, 0.0f, 0.0f));
        creditsText.push_back(TextTimer("From Eletronic Murals", 1000.0f, 1000.0f, 1.0f));
        creditsText.push_back(TextTimer("Thank Can Can Wonderland ...", 1000.0f, 2 * 1000.0f, 2.0f));
        creditsText.push_back(TextTimer("... For their support of the Arts!", 1000.0f, 3 * 1000.0f, 3.0f));
    }
    else {
        //bugbug add more stuff
        std::string s;
        switch ((int)ofRandom(0, 17)) {
        case 0:
            s = "See if they will get you a beer...";
            break;
        case 1:
            s = "Time to ask for a cookie?";
            break;
        case 2:
            s = "Is it bar time?";
            break;
        case 3:
            s = "Want to play a game?";
            break;
        case 4:
            s = "Hal, is that you? Hal? Hal? Way in the back";
            break;
        case 5:
            s = "Maybe go play some pinball";
            break;
        case 6:
            s = "I do not have that many random strings";
            break;
        case 7:
            s = "<your joke here>";
            break;
        case 8:
            s = "Can Can Wonderland Rules!";
            break;
        case 9:
            s = "0101010101010101001011010101001101001001!";
            break;
        case 10:
            s = "A Squared + B Squared does not really = C squared";
            break;
        case 11:
            s = "So what's your OS?";
            break;
        case 12:
            s = "E = MC Squared? what? no way";
            break;
        case 13:
            s = "I’d like to help you out today.Which way did you come in ?";
            break;
        case 14:
            s = "Have a nice day? DON'T TELL ME WHAT TO DO!";
            break;
        case 15:
            s = "HippopotomonstrosesquiPedaliophobia is the fear of long words. How bad is that?";
            break;
        case 16:
            s = "Beer can reduce a hangover";
            break;
        default:
            s = "Take the default action where ever you can";
            break;
        }
        TextTimer t(s, 1500.0f, 1500.0f, 0.0f);
        creditsText.push_back(t);
    }

}

void Eye::setup(const string&texName) {
    if (ofLoadImage(*this, texName)) {
        ofLogNotice("Eye") << "loaded " << texName;
    }
    else {
        ofLogError("Eye") << "not loaded " << texName;
    }
    //assimp not supported model.loadModel(objName);
}
void Eye::start() {
    //color.applyCurrentColor();
    material.begin();
    bind();
}
void Eye::stop() {
    unbind();
    material.end();
}
void SuperSphere::setup(const string&name, const string&blinkPath) {
    blink.push_back(Eye(name)); // element 0 is the main non blinking eye

    if (blinkPath.size() > 0L) {
        blinkingEnabled = true;
        ofDirectory dir(blinkPath + ".blink");
        dir.listDir();
        for (size_t i = 0; i < dir.size(); i++) {
            blink.push_back(Eye(dir.getPath(i)));
        }
        blinker.reset(0.0f);
        blinker.setCurve(LINEAR);
        blinker.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
        blinker.setDuration(3.2f);
        blinker.animateTo(blink.size() - 1);
    }

    setResolution(21);
    panDeg(180);
    // animateToAfterDelay
}
void SuperSphere::update() {
    if (blinkingEnabled) {
        blinker.update(1.0f / ofGetTargetFrameRate());
        if (!blinker.isOrWillBeAnimating()) {
            blinker.reset(0.0f);
            blinker.animateToAfterDelay(blink.size() - 1, ofRandom(15, 415));
        }
    }
}
void SuperSphere::draw() {
    int index = 0; // the non blink index
    if (blinkingEnabled) {
        index = blinker.getCurrentValue();
    }
    blink[index].start();
    ofSpherePrimitive::draw();
    blink[index].stop();
}
