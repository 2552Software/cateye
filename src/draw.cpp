#include "ofApp.h"

void ImageAnimator::draw() {
    rotate(currentRotation); // rotate screen, not object, so same rotation works every time
    mainEyes.draw();
}
void SuperSphere::draw() {
    eye.start();
    ofSpherePrimitive::draw();
    eye.stop();
}
void Eyes::draw() {
    if (isAnimating()) {
        for (auto& eye : eyes) { // keep all eyes in sync to make it easier
            glm::vec3 pos = eye.getPosition();
            pos.z = getAnimator().val();
            eye.setPosition(pos);
            if (rotate) {
                eye.rotateDeg(rotate, 0.0f, 0.0f, 1.0f); //bugbug animate and menu degree bugbug make a point rotate x,y,z?
            }
        }
    }
    getCurrentEyeRef().draw();
}
void ImageAnimator::draw(const std::string& s, float x, float y) {
    font.drawStringAsShapes(s, x, y);
}

void ContoursBuilder::draw(float cxScreen, float cyScreen) {
    ofNoFill();
    ofSetLineWidth(1);// ofRandom(1, 5));
    for (auto& blob : contourDrawer.blobs) {
        ofPolyline line;
        for (int i = 0; i < blob.nPts; i++) {
            line.addVertex((cameraWidth - blob.pts[i].x), blob.pts[i].y);
        }
        line.close();
        line.scale(cxScreen / cameraWidth, cyScreen / cameraHeight);
        line.draw();
    }
    ofSetLineWidth(5);// ofRandom(1, 5));
    if (contourFinder.blobs.size() > 0) {
        for (auto& blob : contourFinder.blobs) {
            ofPolyline line;
            for (int i = 0; i < blob.nPts; i++) {
                line.addVertex((cameraWidth - blob.pts[i].x), blob.pts[i].y);
            }
            line.close();
            line.scale(cxScreen / cameraWidth, cyScreen / cameraHeight);
            line.draw();
        }
    }
}


// see if anything is going on
bool ImageAnimator::isAnimating() {
    if (!rotatingEyes.isAnimating() && !mainEyes.isAnimating()) {
        for (auto& credit : displayText) {
            if (credit.isRunningOrWaitingToRun()) {
                return true;
            }
            else if (credit.lasting.isAnimating()) {
                return true;
            }
        }
        return false;
    }
    return true;
}
bool ImageAnimator::drawText() {
    bool found = false;
    float y = ofGetScreenWidth() / 3;
    for (auto& credit : displayText) {
        if (credit.isRunningOrWaitingToRun()) {
            std::string s;
            if (credit.getString(s)) {
                found = true;
                draw(s, -font.stringWidth(s) / 2, y - font.getLineHeight()*credit.line * 6);
            }
        }
        else if (credit.lasting.isAnimating()) {
            found = true;
            ofSetColor(credit.lasting.getCurrentColor());
            draw(credit.text, -font.stringWidth(credit.text) / 2, y - font.getLineHeight()*credit.line * 6);
        }
    }
    if (!found) {
        // send envent that we are done
        TextEvent args;
        ofNotifyEvent(textFinished, args, this);
    }

    return found;
}
void GameItem::draw() {
    myeye.start();
    if (level == 3) {
        cylinder.draw();
    }
    else if (level == 2) { // 2nd level is boxes
        box.draw();
    }
    else {
        sphere.draw();
    }
    myeye.stop();
}

void ImageAnimator::drawGame() {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(0.0f, ofGetScreenHeight() / 20);
    ofEnableAlphaBlending();
    for (auto& item : gameItems) {
        item.draw();
    }
    ofDisableAlphaBlending();
    ofPopMatrix();
    ofPopStyle();
}

void ImageAnimator::drawContours(float cxScreen, float cyScreen) {
    contours.draw(cxScreen, cyScreen);
}
