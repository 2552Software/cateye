#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    hideMenu = true;
  
    //ofEnableSeparateSpecularLight();
    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLogToConsole();
    //ofEnableLighting();
    ofSetVerticalSync(true);
    ofDisableArbTex();
    //ofSetSmoothLighting(true);
    camera.setup();
    // camera.lookAt(eyeAnimator.sphere);
    light.setup();

    // Works like shit on 4k a does most of OF.
    gui.setup();
    gui.setBackgroundColor(ofColor::white);
    gui.setFillColor(ofColor::blue);

    // setup(const std::string& collectionName = "", const std::string& filename = ofxPanelDefaultFilename, float x = 10, float y = 10);

    gui.add(squareCount.setup("Squares", 10, 10, 100));
    gui.add(squareCount.setup("Triggers", 10.0f, 200.0f, 100.0f));
    gui.add(squareCount.setup("Shapes", 100.0f, 100.0f, 500.f));

    eyeAnimator.setCount(squareCount);
    eyeAnimator.setup();
    gui.loadFont(OF_TTF_SANS, 24, true, true);
    gui.setBorderColor(ofColor::yellow);
    gui.setTextColor(ofColor::black);
    gui.setHeaderBackgroundColor(ofColor::orangeRed);
    gui.setBackgroundColor(ofColor::yellowGreen);
    gui.setPosition(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
    gui.setShape(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2, ofGetScreenWidth() / 10, ofGetScreenHeight() / 10 );
    gui.loadFromFile("settings.xml");
    squareCount.addListener(this, &ofApp::squareCountChanged);
}


void ofApp::shapeSizeChanged(float &size) {
    eyeAnimator.setTriggerCount(size);
}

void ofApp::triggerCountChanged(float &count) {
    eyeAnimator.setTriggerCount(count);
}

void ofApp::squareCountChanged(int &squareCount) {
    eyeAnimator.setCount(squareCount); 
}

//--------------------------------------------------------------
void ofApp::update(){
    eyeAnimator.update();
    light.update();
    //too much only use when needed camera.setDistance(eyeAnimator.camera.getCurrentValue());
    // light.setPosition(0, 0, eyeAnimator.camera.getCurrentValue() - 1500);
    // debug helper
    std::stringstream ss;
    //ss << camera.getDistance();
   // ofSetWindowTitle(ss.str());

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(ofColor::black);
    ofSetColor(ofColor::white);
    if (hideMenu) {
        ofEnableDepthTest();
        ofPushStyle();
        light.enable();
        camera.begin();
        ofPushMatrix();
        ofTranslate((ofGetWidth() / 2) - eyeAnimator.getCurrentEyeRef().getRadius(), ofGetHeight() / 2 - eyeAnimator.getCurrentEyeRef().getRadius() / 2, 0);
        eyeAnimator.draw();
        ofPopMatrix();
        camera.end();
        eyeAnimator.drawContours(ofGetScreenWidth(), ofGetScreenHeight());
        light.disable();
        ofPopStyle();
        ofPopMatrix();
        ofDisableDepthTest();
    }
    else  {
        gui.draw();
    }
}

void ofApp::keyPressed(int key) {
    if (key == 'm') {
        gui.saveToFile("settings.xml");
        hideMenu = !hideMenu;
    }
    else if (key == 's') {
         gui.saveToFile("settings.xml");
        hideMenu = !hideMenu;
    }
    else if (key == 'l') {
        gui.loadFromFile("settings.xml");
    }
}



