#include "ofApp.h"

TextTimer::TextTimer(const std::string& textIn, float timeToRenderIn, float delay, float xIn, float yIn) {
    timeDelay = 0;
    done = false;
    text = textIn;
    timeSet = true;
    timeToRender = timeToRenderIn;
    timeDelay = delay;
    timeBegan = (int)ofGetElapsedTimeMillis();
    x = xIn;
    y = yIn;
    holdTextTime = 35.0f;
}

void ImageAnimator::circle() {
    ofxAnimatableOfPoint point;
    point.setPosition(currentLocation);
    point.setDuration(1.20f);
    point.animateTo(ofVec3f(1000, 1000, 10));
    path.addTransition(point);
    point.animateTo(ofVec3f(1000, 2000, 200));
    path.addTransition(point);
    point.animateTo(ofVec3f(2000, 2000, -200));
    path.addTransition(point);
    point.animateTo(ofVec3f(1000, 1000, 3000));
    path.addTransition(point);
    point.animateTo(ofVec3f(00, 00));
    path.addTransition(point);
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
            lasting.setDuration(5.0f);
            lasting.setRepeatType(PLAY_ONCE);
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
void ImageAnimator::credits() {
    creditsText.clear();

    std::string s = "Tom And Mark";
    float f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 0.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6)));

    s = "From Eletronic Murals";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 10));

    s = "Would like to Thank Can Can Wonderland ...";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 2 * 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 20));

    s = "... for their support of the Arts!";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 3 * 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 30));

    //bugbug add more stuff
    if (ofRandom(10.0f) > 5.0f) {
        s = "Now go see if they will get you a beer...";
    }
    else {
        s = "Go ask for a cookie...";
    }
    f = font.stringWidth(s);
    TextTimer t(s, 1500.0f, 5 * 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 40);
    t.holdTextTime = 35.0f;
    creditsText.push_back(t);

    s = "Good bye!";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 6 * 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 20));
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
    blinkingEnabled = true;
    eye.setup(name);
    string path = DATAPATH;
    path += "\\" + blinkPath + ".blink";
    ofDirectory dir(path);
    dir.listDir();
    blink.push_back(Eye(name));
    std::string name2;
    for (size_t i = 0; i < dir.size(); i++) {
        name2 = path + "\\";
        name2 += dir.getName(i);
        blink.push_back(Eye(name2));
    }
    blink.push_back(Eye(name2)); // last one gets skipped

    blinker.reset(0.0f);
    blinker.setCurve(LINEAR);
    blinker.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    blinker.setDuration(3.2f);
    blinker.animateTo(blink.size() - 1);
    // blink
    setResolution(21);
    panDeg(180);
    // animateToAfterDelay
}
void SuperSphere::update() {
    blinker.update(1.0f / ofGetTargetFrameRate());
    if (!blinker.isOrWillBeAnimating()) {
        blinker.reset(0.0f);
        blinker.animateToAfterDelay(blink.size() - 1, ofRandom(15, 415));
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
