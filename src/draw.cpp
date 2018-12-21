#include "ofApp.h"

void Eyes::rotate(ofVec3f r) {
    for (auto& eye : eyes) {
        eye.currentRotation = r;
    }

}
void Game::draw() {
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
bool Game::isAnimating() {
    if (!rotatingEyes.isAnimating() && !mainEyes.isAnimating()) {
        for (auto& credit : text.fullScreenText) {
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
void Game::drawRotatingEyes() {
    if (rotatingEyes.isAnimating()) {
        rotatingEyes.draw();
    }

}

void TextEngine::draw() {
    float w = ofGetWidth();
    if (fullScreenText.size() > 0) {
        bool found = false;
        float y = w / 3;
        ofPushMatrix();
        ofTranslate(w / 2, ofGetHeight() / 2, getRadius());
        for (auto& text : fullScreenText) {
            if (text.isRunningOrWaitingToRun()) {
                std::string s;
                if (text.getString(s)) {
                    found = true;
                    font.drawStringAsShapes(s, -font.stringWidth(s) / 2, y - font.getLineHeight()*text.line * 6);
                }
            }
            else if (text.lasting.isAnimating()) {
                found = true;
                ofSetColor(text.lasting.getCurrentColor());
                font.drawStringAsShapes(text.text, -font.stringWidth(text.text) / 2, y - font.getLineHeight()*text.line * 6);
            }
        }
        ofPopMatrix();
        if (!found) {
            // send envent that we are done
            call(false);
        }
    }
    if (inlineText.size() > 0) {
        bool found = false;
        float y = w / 3;
        ofPushMatrix();
        ofTranslate(w / 2, ofGetHeight() / 2, getRadius());
        for (auto& text : inlineText) {
            if (text.isRunningOrWaitingToRun()) {
                std::string s;
                if (text.getString(s)) {
                    found = true;
                    font.drawStringAsShapes(s, -font.stringWidth(s) / 2, y - font.getLineHeight()*text.line * 6);
                }
            }
            else if (text.lasting.isAnimating()) {
                found = true;
                ofSetColor(text.lasting.getCurrentColor());
                font.drawStringAsShapes(text.text, -font.stringWidth(text.text) / 2, y - font.getLineHeight()*text.line * 6);
            }
        }
        ofPopMatrix();
        if (!found) {
            // send envent that we are done
            call(true);
        }
    }

}

bool Game::drawText() {
    if (text.isAnimating()) {
        text.draw();
        return true;
    }
    return false;
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

void Game::drawGame() {
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

void Game::drawContours() {
    contours.draw(w, h, getRadius());
}
