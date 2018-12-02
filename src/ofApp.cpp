#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //ofEnableSeparateSpecularLight();
    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
    ofSetBackgroundColor(ofColor::black);
    ofSetColor(ofColor::white);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLogToConsole();
    //ofEnableLighting();
    ofEnableDepthTest();
    ofSetVerticalSync(true);
    ofDisableArbTex();
    //ofSetSmoothLighting(true);
    ofDisableAlphaBlending();
    camera.setup();
    // camera.lookAt(eyeAnimator.sphere);
    eyeAnimator.setup();
    light.setup();

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
    ofPushStyle();
    light.enable();
    camera.begin();
    ofPushMatrix();
    ofTranslate((ofGetWidth() / 2) - eyeAnimator.getCurrentEyeRef().getRadius(), ofGetHeight() / 2 - eyeAnimator.getCurrentEyeRef().getRadius() / 2, 0);
    eyeAnimator.draw();
    ofPopMatrix();
    camera.end();
    eyeAnimator.contours.draw(ofGetScreenWidth(), ofGetScreenHeight());
    light.disable();
    ofPopStyle();
    ofPopMatrix();

}
