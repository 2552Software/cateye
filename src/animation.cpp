#include "ofApp.h"

void ofxAnimatableQueueofVec3f::update() {
    currentAnimation.update(1.0f / ofGetTargetFrameRate());
    if (playing) {
        if (currentAnimation.hasFinishedAnimating()) {
            if (animSteps.size() > 0) {
                //animSteps.erase(animSteps.begin());
                currentAnimation = animSteps.front();
                animSteps.pop_front();
                ofLogNotice() << "next";
            }
            else {
                ofLogNotice() << "empty";
            }
        }
    }
}
void ofxAnimatableQueueofVec3f::addTransition(ofxAnimatableOfPoint targetValue) {
    ofLogNotice() << "addTransition " << targetValue.getCurrentPosition();
    if (targetValue.getCurrentPosition().x == 0) {
        int i = 0;
    }
    if (animSteps.size() > maxListSize) {
        ofLogNotice() << " cap list size to maxListSize " << maxListSize;
        animSteps.pop_back(); // only keep the  most recent
    }
    animSteps.push_back(targetValue);//bugbug go to pointer
}
void ofxAnimatableQueueofVec3f::insertTransition(ofxAnimatableOfPoint targetValue, bool forceNext) {
    if (forceNext) {
        currentAnimation = targetValue; // make this one current, drop the current one
    }
    else {
        ofLogNotice() << "insertTransition " << targetValue.getCurrentPosition();
        if (animSteps.size() > maxListSize) { // make const
            ofLogNotice() << " cap list size to maxListSize " << maxListSize;
            animSteps.pop_back(); // only keep the  most recent
        }
        animSteps.push_front(targetValue);//bugbug go to pointer
    }
}
bool ofxAnimatableQueueofVec3f::hasFinishedAnimating() {
    return currentAnimation.hasFinishedAnimating();
}
ofxAnimatableOfPoint ofxAnimatableQueueofVec3f::getCurrentValue() {
    return currentAnimation;
}
ofPoint ofxAnimatableQueueofVec3f::getPoint() {
    return currentAnimation.getCurrentPosition();
}
void ofxAnimatableQueueofVec3f::append(const ofVec3f& target) {
    ofxAnimatableOfPoint targetPoint;
    if (animSteps.size() > 0) {
        targetPoint.setPosition(getPoint());
    }
    targetPoint.animateTo(target);
    targetPoint.setDuration(1.25);
    targetPoint.setRepeatType(PLAY_ONCE);
    targetPoint.setCurve(LINEAR);
    addTransition(targetPoint);
}
