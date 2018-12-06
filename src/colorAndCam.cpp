#include "ofApp.h"

void Light::setup() {
    ofLight::setup();
    setDirectional();
    setOrientation(ofVec3f(-200.0f, 300.0f, 00.0f));
    setPosition(0, 0, 2000);

    specularcolor.setColor(ofColor::mediumVioletRed);
    specularcolor.setDuration(1.0f);
    specularcolor.setRepeatType(LOOP_BACK_AND_FORTH);
    specularcolor.setCurve(LINEAR);
    specularcolor.animateTo(ofColor::orangeRed);

    ambientcolor.setColor(ofColor::blue);
    ambientcolor.setDuration(1.5f);
    ambientcolor.setRepeatType(LOOP_BACK_AND_FORTH);
    ambientcolor.setCurve(LINEAR);
    ambientcolor.animateTo(ofColor::red);
}
void Light::update() {
    specularcolor.update(1.0f / ofGetTargetFrameRate());
    ambientcolor.update(1.0f / ofGetTargetFrameRate());

    setAmbientColor(ambientcolor.getCurrentColor());
    setSpecularColor(specularcolor.getCurrentColor());
    //setAmbientColor(ofColor::mediumVioletRed);
    ///setSpecularColor(ofColor::saddleBrown);
    ////setDiffuseColor(ofColor::pink);
}
void Light::setOrientation(ofVec3f rot) {
    ofVec3f xax(1, 0, 0);
    ofVec3f yax(0, 1, 0);
    ofVec3f zax(0, 0, 1);
    ofQuaternion q;
    q.makeRotate(rot.x, xax, rot.y, yax, rot.z, zax);
    ofLight::setOrientation(q);
}

void Material::setup() {
    color.setColor(ofColor::white);
    color.setDuration(1.0f);
    color.setRepeatType(LOOP_BACK_AND_FORTH);
    color.setCurve(LINEAR);
    color.animateTo(ofColor::orangeRed);// COLOR not used yet

    setShininess(120);
    setSpecularColor(ofColor::white);
    // setEmissiveColor(ofColor::black);
    setDiffuseColor(ofColor::whiteSmoke);
    setAmbientColor(ofColor::navajoWhite);
}