#include "ofApp.h"

void Eyes::rotate(ofVec3f r) {
    for (auto& eye : eyes) {
        eye.currentRotation = r;
    }

}
void ImageAnimator::draw() {
    //ofLogNotice() << "rotate to targert" << target;
    if (!drawText()) { // draw text first and give it the full screen
        if (rotatingEyes.isAnimating()) {
            drawRotatingEyes();
        }
        else {
            setTitle();
            mainEyes.rotate(currentRotation);
            mainEyes.draw();
            if (!mainEyes.isAnimating()) {
                drawContours();
                blink();
            }
            drawGame(); // draw any game that may be running
        }
    }
}
void SuperSphere::draw() {
    eye.start();
    ofSpherePrimitive sphere;
    r = std::min(ofGetWidth(), ofGetHeight()) / 2;
    // for heavy debug ofLogNotice() << "Radius" << r << " W " << ofGetWidth() << " H " << ofGetHeight();
    sphere.setRadius(r);
    sphere.setPosition((ofGetWidth() / 2), (ofGetHeight() / 2), 0);
    sphere.panDeg(180);
    sphere.rotateDeg(currentRotation.x, 1.0f, 0.0f, 0.0f);
    sphere.rotateDeg(currentRotation.y, 0.0f, 1.0f, 0.0f);
    sphere.setResolution(27);
    sphere.draw();
    eye.stop();
}
void Eyes::draw() {
    if (isAnimating()) {
        for (auto& eye : eyes) { // keep all eyes in sync to make it easier
           // glm::vec3 pos = eye.getPosition();
            //pos.z = getAnimator().val();
            //bugbug do later eye.setPosition(pos);//bug does this break animiation?
        }
    }
    getCurrentSphereRef().draw();
}
void ImageAnimator::draw(const std::string& s, float x, float y) {
    font.drawStringAsShapes(s, x, y);
}

void ContoursBuilder::draw(float w, float h, float z) {
    ofNoFill();
    ofSetLineWidth(1);// ofRandom(1, 5));
    for (auto& blob : contourDrawer.blobs) {
        ofPolyline line;
        for (int i = 0; i < blob.nPts; i++) {
            line.addVertex((cameraWidth - blob.pts[i].x), blob.pts[i].y, z);
        }
        line.close();
        line.scale(w / cameraWidth, h / cameraHeight);
        line.draw();
    }
    ofSetLineWidth(5);// ofRandom(1, 5));
    if (contourFinder.blobs.size() > 0) {
        for (auto& blob : contourFinder.blobs) {
            ofPolyline line;
            for (int i = 0; i < blob.nPts; i++) {
                line.addVertex((cameraWidth - blob.pts[i].x), blob.pts[i].y, z);
            }
            line.close();
            line.scale(w / cameraWidth, h / cameraHeight);
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
void ImageAnimator::drawRotatingEyes() {
    if (rotatingEyes.isAnimating()) {
        rotatingEyes.draw();
    }

}
bool ImageAnimator::drawText() {
    bool found = false;
    if (displayText.size() > 0) {
        ofPushMatrix();
        ofTranslate(0.0f, 0.0f, -getRadius());
        float y = (w) ? w : 1 / 3;
        for (auto& text : displayText) {
            if (text.isRunningOrWaitingToRun()) {
                std::string s;
                if (text.getString(s)) {
                    found = true;
                    draw(s, -font.stringWidth(s) / 2, y - font.getLineHeight()*text.line * 6);
                }
            }
            else if (text.lasting.isAnimating()) {
                found = true;
                ofSetColor(text.lasting.getCurrentColor());
                draw(text.text, -font.stringWidth(text.text) / 2, y - font.getLineHeight()*text.line * 6);
            }
        }
        ofPopMatrix();
        if (!found) {
            // send envent that we are done
            TextEvent args;
            ofNotifyEvent(textFinished, args, this);
        }
    }
    return found;
}
void GameItem::draw() {
    myeye.start();
    switch (level) {
    case Basic:
        sphere.draw();
        break;
    case Medium:
        box.draw();
        break;
    case Difficult:
        cylinder.draw();
        break;
    case EndGame:
        box.draw();
        break;
    }
    myeye.stop();
}

void ImageAnimator::drawGame() {
    ofPushStyle();
    ofPushMatrix();
   //bugbug whjat is this? ofTranslate(0.0f, w / 20);
    ofEnableAlphaBlending();
    for (auto& item : gameItems) {
        item.draw();
    }
    ofDisableAlphaBlending();
    ofPopMatrix();
    ofPopStyle();
}

void ImageAnimator::drawContours() {
    contours.draw(w, h, getRadius());
}
