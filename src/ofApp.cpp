#include "ofApp.h"
#include "sound.h"
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("Robocat");
    //ofSetFullscreen(true);
    hideMenu = true;
    ofSetFrameRate(30.0f); // camers 30 so why go higher?
    
    eyeAnimator.music.setup(cameraWidth, cameraHeight); // tie to app

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

    gui.add(squareCount.setup("Squares", 15, 15, 100));
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
void ofApp::update() {
    eyeAnimator.update();
    light.update();
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetBackgroundColor(ofColor::black);
    ofSetColor(ofColor::white);
    if (hideMenu) {
        ofEnableDepthTest();
        ofPushStyle();
        ofPushMatrix();
        light.enable();
        camera.begin();
        bool textOnly = eyeAnimator.drawText();
        bool eyeDrawn = false;
        if (!textOnly) { // draw text first, if no text draw the eye
                // show spirl eye first, if its not running try to text if al else fails show the main eye
            if (eyeAnimator.rotatingEyes.isAnimating()) {
                eyeAnimator.rotatingEyes.draw();
            }
            else {
                eyeAnimator.draw();
                eyeDrawn = true;
            }
        }

        ofPopMatrix();
        ofPopStyle();
        camera.end();
        ofPushStyle();
        ofPushMatrix();

        // if nothing else is going on draw motion outside of camera but in light

        if (!textOnly && !eyeAnimator.isAnimating()) {
            eyeAnimator.drawContours(ofGetScreenWidth(), ofGetScreenHeight());
        }
        if (!textOnly) {
            eyeAnimator.drawGame(); // draw any game that may be running
        }
        ofPopMatrix();
        ofPopStyle();
        light.disable();
        ofDisableDepthTest();
        // need pure light/color and no camera
        if (!textOnly) {
            if (eyeAnimator.inGame() && eyeAnimator.winnerHitCount() > 0) {
                eyeAnimator.setTitle();
            }
            else if (eyeDrawn) { // blink as needed
                eyeAnimator.blink();
            }
        }
    }
    else  {
        gui.draw();
    }
}
void ofApp::keyReleased(int key) {
    // sends key messages to ofxPDSPComputerKeyboard
    eyeAnimator.music.keyboard.keyReleased(key);
}
void ofApp::keyPressed(int key) {
    eyeAnimator.music.keyboard.keyPressed(key);
    return;//bugbug
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

    // we can launch our sequences with the launch method, with optional quantization
    switch (key) {
    case '1':
        eyeAnimator.music.engine.sequencer.sections[0].launch(0, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case '2':
        eyeAnimator.music.engine.sequencer.sections[0].launch(1, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case '3':
        eyeAnimator.music.engine.sequencer.sections[0].launch(2, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case '4':
        eyeAnimator.music.engine.sequencer.sections[0].launch(3, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case '6':
        eyeAnimator.music.quantize = true;
        eyeAnimator.music.quantime = 1.0;
        break;
    case '7':
        eyeAnimator.music.quantize = true;
        eyeAnimator.music.quantime = 1.0 / 4.0;
        break;
    case '8':
        eyeAnimator.music.quantize = true;
        eyeAnimator.music.quantime = 1.0 / 8.0;
        break;
    case '9':
        eyeAnimator.music.quantize = true;
        eyeAnimator.music.quantime = 1.0 / 16.0;
        break;
    case '0':
        eyeAnimator.music.quantize = false;
        break;
    case 'q':
        eyeAnimator.music.engine.sequencer.sections[1].launch(0, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case 'w':
        eyeAnimator.music.engine.sequencer.sections[1].launch(1, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case 'e':
        eyeAnimator.music.engine.sequencer.sections[1].launch(2, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case 'r':
        eyeAnimator.music.engine.sequencer.sections[1].launch(3, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case 't':
        eyeAnimator.music.engine.sequencer.sections[1].launch(-1, eyeAnimator.music.quantize, eyeAnimator.music.quantime);
        break;
    case 'a':
        eyeAnimator.music.seq_mode = eyeAnimator.music.seq_mode ? 0 : 1;
        switch (eyeAnimator.music.seq_mode) {
        case 0:
            for (int i = 0; i < 4; ++i) {
                eyeAnimator.music.engine.sequencer.sections[1].oneshot(i);
            }
            break;
        case 1:
            for (int i = 0; i < 4; ++i) {
                eyeAnimator.music.engine.sequencer.sections[1].loop(i);
            }
            break;
        }
        break;
    case ' ': // pause / play
        if (eyeAnimator.music.engine.sequencer.isPlaying()) {
            eyeAnimator.music.engine.sequencer.pause();
        }
        else {
            eyeAnimator.music.engine.sequencer.play();
        }
        break;
    case 's': // stop
        eyeAnimator.music.engine.sequencer.stop();
        break;
    }

    switch (key) {
    case '5': // select one shot / loop pattern behavior
        if (eyeAnimator.music.oneShot) {
            eyeAnimator.music.engine.sequencer.sections[0].setChange(0, pdsp::Behavior::Next);
            eyeAnimator.music.engine.sequencer.sections[0].setChange(1, pdsp::Behavior::Next);
            eyeAnimator.music.engine.sequencer.sections[0].setChange(2, pdsp::Behavior::Next);
            eyeAnimator.music.engine.sequencer.sections[0].setChange(3, pdsp::Behavior::Next);
            eyeAnimator.music.oneShot = false;
        }
        else {
            eyeAnimator.music.engine.sequencer.sections[0].setChange(0, pdsp::Behavior::Nothing);
            eyeAnimator.music.engine.sequencer.sections[0].setChange(1, pdsp::Behavior::Nothing);
            eyeAnimator.music.engine.sequencer.sections[0].setChange(2, pdsp::Behavior::Nothing);
            eyeAnimator.music.engine.sequencer.sections[0].setChange(3, pdsp::Behavior::Nothing);
            eyeAnimator.music.oneShot = true;
        }
        break;
    }
}




