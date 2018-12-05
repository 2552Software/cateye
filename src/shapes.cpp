#include "ofApp.h"

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
    blinker.setDuration(0.2f);
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
        blinker.animateToAfterDelay(blink.size() - 1, ofRandom(5));
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
