#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    hideMenu = true;
    ofSetFrameRate(30.0f); // camers 30 so why go higher?

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
    gui.add(maxForTrigger.setup("Triggers", 50.0f, 200.0f, 100.0f));
    gui.add(maxForShape.setup("Shapes", 100.0f, 200.0f, 500.f));

    eyeAnimator.setCount(squareCount);
    eyeAnimator.setShapeMinSize(maxForShape);
    eyeAnimator.setTriggerCount(maxForTrigger);
    eyeAnimator.setup();
    //bugbug eyeAnimator.credits(true); // setup credits, shown at boot bugbug spin eyes at boot too -- and restore text

    gui.loadFont(OF_TTF_SANS, 24, true, true);
    gui.setBorderColor(ofColor::yellow);
    gui.setTextColor(ofColor::black);
    gui.setHeaderBackgroundColor(ofColor::orangeRed);
    gui.setBackgroundColor(ofColor::yellowGreen);
    gui.setPosition(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
    gui.setShape(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2, ofGetScreenWidth() / 10, ofGetScreenHeight() / 10 );
    gui.loadFromFile("settings.xml");
    squareCount.addListener(this, &ofApp::squareCountChanged);
    maxForTrigger.addListener(this, &ofApp::triggerCountChanged);
    maxForShape.addListener(this, &ofApp::shapeSizeChanged);

}

void ofApp::shapeSizeChanged(float &size) {
    // menu is next release bugbugeyeAnimator.setShapeMinSize(size);
}

void ofApp::triggerCountChanged(float &count) {
    // menu is next release bugbugeyeAnimator.setTriggerCount(count);
}

void ofApp::squareCountChanged(int &squareCount) {
    // menu is next release bugbug eyeAnimator.setCount(squareCount); 
}

//--------------------------------------------------------------
void ofApp::update(){
    eyeAnimator.update();
    light.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(ofColor::black);
    ofSetColor(ofColor::white);
    if (hideMenu) {
        ofEnableDepthTest();
        ofPushStyle();
        ofPushMatrix();
        light.enable();
        camera.begin();

        if (!eyeAnimator.drawText()) { // draw text first, if no text draw the eye
                // show spirl eye first, if its not running try to text if al else fails show the main eye
            if (eyeAnimator.rotatingEyes.isAnimating()) {
                eyeAnimator.rotatingEyes.draw();
            }
            else {
                eyeAnimator.draw();
            }
        }

        ofPopMatrix();
        ofPopStyle();
        camera.end();

        // if nothing else is going on draw motion outside of camera but in light
        ofPushStyle();
        ofPushMatrix();
        if (!eyeAnimator.isAnimating()) {
            eyeAnimator.drawContours(ofGetScreenWidth(), ofGetScreenHeight());
        }
        eyeAnimator.drawGame(); // draw any game that may be running
        ofPopMatrix();
        ofPopStyle();
        light.disable();
        ofDisableDepthTest();

        if (eyeAnimator.inGame() && eyeAnimator.winnerHitCount() > 0) {
            std::stringstream ss;
            ss << eyeAnimator.winnerHitCount() << " of " << eyeAnimator.winnerThreshold();
            std::string s = "Game On! Find ";
            s += ss.str();
            ofSetColor(ofColor::white);
            eyeAnimator.font.drawStringAsShapes(s, ofGetScreenWidth() / 2 - eyeAnimator.font.stringWidth(s)/2, eyeAnimator.font.getLineHeight() * 5);
        }

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



