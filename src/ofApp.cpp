#include "ofApp.h"

class Scheduler : public ofThread {
public:
    Scheduler() {
        timer.setPeriodicEvent(1000000000*60); // this is 1 second in nanoseconds
        startThread();
    }

private:
    ofTimer timer;
    void threadedFunction() {
        while (isThreadRunning()) {
            timer.waitNext();
            // Do your thing here. It will run once per 60 seconds.
        }
    }
};

/*
 *  ofxTextWriter.h
 *  CFText
 *
 *  Created by Artem Titoulenko on 1/6/10.
 *  Copyright 2010 Artem Titoulenko. All rights reserved.
 *
 */

class ofxTextWriter {
    string text;
    float timeToRender;
    float timeBegan;
    bool  timeSet, done;

public:

    //for use if you just want a blank TextWriter, for whatever reason.
    void init() {
        text = "";
        timeToRender = 0;
        timeSet = done = false;
    }

    ofxTextWriter() {
        init();
    }

    ofxTextWriter(string _text, float _timeToRender = 5) {
        text = _text;
        timeToRender = _timeToRender;
        done = false;
    }

    string whatToRender() {
        if (!done) {
            if (!timeSet) {
                timeBegan = ofGetElapsedTimef();
                timeSet = true;
            }

            //Lets not draw it, that's too many resources we dont have.
            //Lets just return what part of the string should be already drawn.
            //That way people can decide what they want to do with it and how.
            int n = (int)((ofGetElapsedTimef() / (timeSet + timeToRender)) * text.length());

            if (n + 1 == (int)text.length()) done = true;

            return text.substr(0, min(n, (int)text.length())); //lets not access outside the buffer, eh?
        }
        else {
            return text;
        }
    }

    /* -------- HELPER FUNCTIONS --------- */
    void resetTime() {
        timeSet = false;
    }

    void setTimeToRender(float _timeToRender) {
        resetTime();
        timeToRender = _timeToRender;
        done = false;
    }

    void setTextToRender(string _text) {
        text = _text;
    }

    bool isDone() {
        return done;
    }
};

//--------------------------------------------------------------
void ofApp::setup(){
    hideMenu = true;
    ofSetFrameRate(60.0f);

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
    eyeAnimator.setShapeMinSize(size);
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



