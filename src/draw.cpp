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
    if (!rotatingEyes.isAnimating() && !mainEyes.isAnimating() && !fancyText.isFullScreenAnimating()) {
        return false;
    }
    return true;
}
void Game::drawRotatingEyes() {
    if (rotatingEyes.isAnimating()) {
        rotatingEyes.draw();
    }

}

void TextEngine::drawShapes(const std::string& s) {
    // get the string as paths
    vector < ofPath > paths = font.getStringAsPoints(s);
    for (size_t i = 0; i < paths.size(); i++) {
        // for every character break it out to polylines
        vector <ofPolyline> polylines = paths[i].getOutline();
        // for every polyline, draw every fifth point
        for (size_t j = 0; j < polylines.size(); j++) {
            for (size_t k = 0; k < polylines[j].size(); k += 5) {         // draw every "fifth" point
                ofDrawCircle(polylines[j][k], 3);
            }
        }
    }
}
bool TextEngine::animateString(TextTimer& text, int x, int y) {
    bool found = false;
    if (text.isRunningOrWaitingToRun()) {
        std::string s;
        if (text.getString(s)) { 
            found = true;
            //text.setColor();
            ofRectangle rect = font.getStringBoundingBox(s, 0.0f, 0.0f);
            font.drawStringAsShapes(s, x- rect.width / 2, y - rect.height*text.getLine() * 6);
        }
    }
    else if (text.isAnimating()) { // get raw text if not running, the full string
        found = true;
        text.setColor();
        ofRectangle rect = font.getStringBoundingBox(text.getRawText(), 0.0f, 0.0f); // full string
        font.drawStringAsShapes(text.getRawText(), x-font.stringWidth(text.getRawText()) / 2, y - font.getLineHeight()*text.getLine() * 6);
    }
    return found;
}
void TextEngine::print(const std::string& s, float x, float y, float z) {
    ofPushMatrix();
    ofRectangle rect = font.getStringBoundingBox(s, 0.0f, 0.0f);
    ofTranslate(x - rect.width / 2, y+rect.height/2, z);
    font.drawStringAsShapes(s, 0, 0);
    ofPopMatrix();

}
void TextEngine::draw(float x, float y, float z) {
    bool found = false;
    if (fullScreenText.size() > 0) {
        ofPushMatrix();
        ofTranslate(x, y, z);
        for (auto& text : fullScreenText) {
            if (animateString(text, 0.0f, 0.0f)) {
                found = true;
            }
        }
        ofPopMatrix();
        if (!found) {
            // send envent that we are done
            call(false);
        }
    }
    if (inlineText.size() > 0) {
        ofPushMatrix();
        ofTranslate(x, y, z); // ned to do the z here as no other api in fonts has a z
        for (auto& text : inlineText) { 
            if (animateString(text, 0.0f, 0.0f)) {
                found = true;
            }
        }
        ofPopMatrix();
        if (!found) {
            // send envent that we are done
            call(true);
        }
    }
}

// return true if full screen mode enabled
bool Game::drawText() {
    basicText.draw(w / 2, w / 2, getRadius());
    fancyText.draw(w/2, w/3, getRadius());
    return fancyText.isFullScreenAnimating();
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
