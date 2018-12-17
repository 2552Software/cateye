#include "ofApp.h"

void ofApp::audioOut(ofSoundBuffer & outBuffer) {
    if (eyeAnimator.winnerHitCount() == 0) {
        return;
    }
    if (eyeAnimator.listOfMusic.size() != 0){
        frequency = eyeAnimator.listOfMusic.back().frequency;
        volume = eyeAnimator.listOfMusic.back().volume;
        eyeAnimator.listOfMusic.pop_back();
    }

    // base frequency of the lowest sine wave in cycles per second (hertz)

    // mapping frequencies from Hz into full oscillations of sin() (two pi)
    float wavePhaseStep = (frequency / outBuffer.getSampleRate()) * TWO_PI;
    float pulsePhaseStep = (0.5 / outBuffer.getSampleRate()) * TWO_PI;

    // this loop builds a buffer of audio containing 3 sine waves at different
    // frequencies, and pulses the volume of each sine wave individually. In
    // other words, 3 oscillators and 3 LFOs.

    for (size_t i = 0; i < outBuffer.getNumFrames(); i++) {

        // build up a chord out of sine waves at 3 different frequencies
        float sampleLow = sin(wavePhase);
        float sampleMid = sin(wavePhase * 1.5);
        float sampleHi = sin(wavePhase * 2.0);

        // pulse each sample's volume
        sampleLow *= sin(pulsePhase);
        sampleMid *= sin(pulsePhase * 1.04);
        sampleHi *= sin(pulsePhase * 1.09);

        float fullSample = (sampleLow + sampleMid + sampleHi);

        // reduce the full sample's volume so it doesn't exceed 1
        fullSample *= 0.3;

        // write the computed sample to the left and right channels
        outBuffer.getSample(i, 0) = fullSample;
        outBuffer.getSample(i, 1) = fullSample;

        // get the two phase variables ready for the next sample
        wavePhase += wavePhaseStep;
        pulsePhase += pulsePhaseStep;
    }

    unique_lock<mutex> lock(audioMutex);
    lastBuffer = outBuffer;
}
//--------------------------------------------------------------
void ofApp::setup(){

    hideMenu = true;
    ofSetFrameRate(30.0f); // camers 30 so why go higher?

    wavePhase = 0;
    pulsePhase = 0;

    // start the sound stream with a sample rate of 44100 Hz, and a buffer
    // size of 512 samples per audioOut() call
    ofSoundStreamSettings settings;
    settings.numOutputChannels = 2;
    settings.sampleRate = 44100;
    settings.bufferSize = 512;
    settings.numBuffers = 4;
    settings.setOutListener(this);
    soundStream.setup(settings);
    //soundStream.printDeviceList();
    volume = 0.1f;
    soundStream.start();

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
void ofApp::update(){
    eyeAnimator.update();
    light.update();
    
    // "lastBuffer" is shared between update() and audioOut(), which are called
    // on two different threads. This lock makes sure we don't use lastBuffer
    // from both threads simultaneously (see the corresponding lock in audioOut())
    unique_lock<mutex> lock(audioMutex);

    // this loop is building up a polyline representing the audio contained in
    // the left channel of the buffer

    // the x coordinates are evenly spaced on a grid from 0 to the window's width
    // the y coordinates map each audio sample's range (-1 to 1) to the height
    // of the window

    rms = lastBuffer.getRMSAmplitude();
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



