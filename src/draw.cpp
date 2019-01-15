#include "ofApp.h"
#include "sound.h"


void SuperSphere::draw() {
    if (sphere.getRadius() > 0) {
        if (currentRotation.x || currentRotation.y) {
            sphere.rotateDeg(currentRotation.x, 1.0f, 0.0f, 0.0f);
            sphere.rotateDeg(currentRotation.y, 0.0f, 1.0f, 0.0f);
        }
        sphere.draw();
        home(); // restore to start position
    }
}

void CrazyEye::draw() {
    if (sphere.getRadius() > 0) {
        sphere.rotateDeg(rotater.val()*15.0f, 0.0f, 0.0f, 1.0f);
        ofScale(animatorUp.getPercentDone()*1.5f);
        sphere.draw();
        home(); // restore to start position
    }
}
void Game::draw(Music*music) {
    if (music) {
        if (current.getSound().sendSound()) {
            // y value controls the trigger intensity
           // float trig = ofMap(y, 0, ofGetHeight(), 1.0f, 0.000001f);
            //music->gate_ctrl.off();
            music->set(current.getSound());
            current.getSound().setSound(false);
            // play everytime an item is selected
            //bugbug figure this out music->gate_ctrl.trigger(current->trigger); // we send a trigger to the envelope
        }
    }

    if (!drawText()) { // draw text first and give it the full screen
        if (rotatingEye.isRunning()) {
            ofPushMatrix(); 
            ofTranslate(w / 2, h / 2, 0.0f);// z will need to be moved via apis since OF is not consistant here
            rotatingEyesSkins.getCurrentRef().start();
            rotatingEye.draw();
            rotatingEyesSkins.getCurrentRef().stop();
            ofPopMatrix();
        }
        else {
            ofPushMatrix();
            ofTranslate(w / 2, h / 2);// z will need to be moved via apis since OF is not consistant here
            setTitle();
            if (!inGame()) {
                mainEye.setRotation(currentRotation);
                mainEyesSkins.getCurrentRef().start();
                mainEye.draw();
                mainEyesSkins.getCurrentRef().stop();
            }
            ofPopMatrix();
            // next items are drawn absolute and are driven by camera and converted to screen units
            drawContours();
            blink();
            drawGame(); // draw any game that may be running
        }
    }
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
            line.scale(xFactor, yFactor);
            line.draw();
        }
    }
}


// see if anything is going on
bool Game::isAnimating() {
    if (!rotatingEye.isAnimating() && !mainEye.isAnimating() && !fancyText.isFullScreenAnimating()) {
        return false;
    }
    return true;
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
// draw until object is deleted
bool TextEngine::animateString(TextTimer& text, int x, int y) {
    std::string s = text.getPartialString();
    if (s.size() > 0) {
        ofRectangle rect = font.getStringBoundingBox(s, 0.0f, 0.0f);
        font.drawStringAsShapes(s, x - rect.width / 2, y + rect.height*text.getLine()+ rect.height/2); // give a little room bettween
        return true;
    }
    return false;
}
void TextEngine::print(const std::string& s, float x, float y, float z) {
    ofPushMatrix();
    ofRectangle rect = font.getStringBoundingBox(s, 0.0f, 0.0f);
    ofTranslate(x - rect.width / 2, y+rect.height/2, z);
    font.drawStringAsShapes(s, 0, 0);
    ofPopMatrix();

}
void TextEngine::draw(float z) {
    bool found = false;
    if (fullScreenText.size() > 0) {
        ofPushMatrix();
        ofTranslate(0.0f, 0.0f, z);
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
        ofTranslate(0.0f, 0.0f, z); // ned to do the z here as no other api in fonts has a z
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
    ofPushMatrix();
    ofTranslate(w / 2, h / 2, 0.0f);// z will need to be moved via apis since OF is not consistant here
    basicText.draw(getRadiusGlobal());
    fancyText.draw(getRadiusGlobal());
    ofPopMatrix();
    return fancyText.isFullScreenAnimating();
}
void LocationToActionMap::draw() {
}

void Game::drawGame() {
    ofPushMatrix();
    ofSetColor(ofColor::hotPink);
    ofSetLineWidth(1);
    ofNoFill();

    for (auto& a : aimationMaps) {
        for (auto& grid : a) {
            if (current.getLevel() == grid.second.getLevel())  {
                ofDrawRectangle(convert(grid.second));
            }
        }
    }
    for (auto a : gameEyes) {
        a.setRotation(currentRotation);
        mainEyesSkins.getCurrentRef().start();
        a.draw();
        mainEyesSkins.getCurrentRef().stop();
    }
    ofPopMatrix();
}

void Game::drawContours() {
    contours.draw(w, h, getRadiusGlobal());
}
