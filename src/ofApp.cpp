#include "ofApp.h"
#include "sound.h"

Music* music=nullptr; // never deleted since its need for the life of the app, nor allocation checked -- let it crash if we cannot get his
AudioPlayer *player = nullptr;


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLogToConsole();

    //ofSetFullscreen(true);
    //ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());

    hideMenu = true;
    ofSetFrameRate(15.0f); 

    ofLogNotice("ofApp::setup") << "of version " << ofGetVersionInfo();

    //ofEnableSeparateSpecularLight();

    //ofEnableLighting();
    ofSetVerticalSync(true);
    ofDisableArbTex();
    //ofSetSmoothLighting(true);
  
    // camera.lookAt(eyeAnimator.sphere);
    light.setup();
    music = new Music;
    music->setup(cameraWidth, cameraHeight); // tie to app

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

    player = new AudioPlayer; //bugbug replace cat calls with this
    player->load("wargames.wav");
    player->out("0") >> music->engine.audio_out(0);
    player->play();


    //bugbug eyeAnimator.credits(true); // setup credits, shown at boot bugbug spin eyes at boot too -- and restore text

    gui.loadFont(OF_TTF_SANS, 24, true, true);
    gui.setBorderColor(ofColor::yellow);
    gui.setTextColor(ofColor::black);
    gui.setHeaderBackgroundColor(ofColor::orangeRed);
    gui.setBackgroundColor(ofColor::yellowGreen);
    gui.setPosition(ofGetWidth() / 2, ofGetHeight() / 2);
    gui.setShape(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 10, ofGetHeight() / 10 );
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
void ofApp::windowResized(int w, int h) {
    eyeAnimator.windowResized(w, h);
    //eyeAnimator.font.

   // camera.setDistance(10.0f);// radius which is y max of screen
}

//--------------------------------------------------------------
void ofApp::update() {
    eyeAnimator.update(music);
    light.update();
    music->update();
    music->setPixels(eyeAnimator.contours.contourFinder);

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
        //camera.begin();
    // show spirl eye first, if its not running try to text if al else fails show the main eye
        eyeAnimator.draw();
        ofPopMatrix();
        ofPopStyle();
       // camera.end();

        light.disable();
        ofDisableDepthTest();
    }
    else  {
        gui.draw();
    }
}
void ofApp::keyReleased(int key) {
    // sends key messages to ofxPDSPComputerKeyboard
    music->keyboard.keyReleased(key);
}
void ofApp::keyPressed(int key) {
    music->keyboard.keyPressed(key);
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
        music->engine.sequencer.sections[0].launch(0, music->quantize, music->quantime);
        break;
    case '2':
        music->engine.sequencer.sections[0].launch(1, music->quantize, music->quantime);
        break;
    case '3':
        music->engine.sequencer.sections[0].launch(2, music->quantize, music->quantime);
        break;
    case '4':
        music->engine.sequencer.sections[0].launch(3, music->quantize, music->quantime);
        break;
    case '6':
        music->quantize = true;
        music->quantime = 1.0;
        break;
    case '7':
        music->quantize = true;
        music->quantime = 1.0 / 4.0;
        break;
    case '8':
        music->quantize = true;
        music->quantime = 1.0 / 8.0;
        break;
    case '9':
        music->quantize = true;
        music->quantime = 1.0 / 16.0;
        break;
    case '0':
        music->quantize = false;
        break;
    case 'q':
        music->engine.sequencer.sections[1].launch(0, music->quantize, music->quantime);
        break;
    case 'w':
        music->engine.sequencer.sections[1].launch(1, music->quantize, music->quantime);
        break;
    case 'e':
        music->engine.sequencer.sections[1].launch(2, music->quantize, music->quantime);
        break;
    case 'r':
        music->engine.sequencer.sections[1].launch(3, music->quantize, music->quantime);
        break;
    case 't':
        music->engine.sequencer.sections[1].launch(-1, music->quantize, music->quantime);
        break;
    case 'a':
        music->seq_mode = music->seq_mode ? 0 : 1;
        switch (music->seq_mode) {
        case 0:
            for (int i = 0; i < 4; ++i) {
                music->engine.sequencer.sections[1].oneshot(i);
            }
            break;
        case 1:
            for (int i = 0; i < 4; ++i) {
                music->engine.sequencer.sections[1].loop(i);
            }
            break;
        }
        break;
    case ' ': // pause / play
        if (music->engine.sequencer.isPlaying()) {
            music->engine.sequencer.pause();
        }
        else {
            music->engine.sequencer.play();
        }
        break;
    case 's': // stop
        music->engine.sequencer.stop();
        break;
    }

    switch (key) {
    case '5': // select one shot / loop pattern behavior
        if (music->oneShot) {
            music->engine.sequencer.sections[0].setChange(0, pdsp::Behavior::Next);
            music->engine.sequencer.sections[0].setChange(1, pdsp::Behavior::Next);
            music->engine.sequencer.sections[0].setChange(2, pdsp::Behavior::Next);
            music->engine.sequencer.sections[0].setChange(3, pdsp::Behavior::Next);
            music->oneShot = false;
        }
        else {
            music->engine.sequencer.sections[0].setChange(0, pdsp::Behavior::Nothing);
            music->engine.sequencer.sections[0].setChange(1, pdsp::Behavior::Nothing);
            music->engine.sequencer.sections[0].setChange(2, pdsp::Behavior::Nothing);
            music->engine.sequencer.sections[0].setChange(3, pdsp::Behavior::Nothing);
            music->oneShot = true;
        }
        break;
    }
}




