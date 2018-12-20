#include "ofApp.h"

void ImageAnimator::draw() {
    //rotate(currentRotation); // rotate screen, not object, so same rotation works every time
    //ofLogNotice() << "rotate to targert" << target;
    ofVec3f current = mainEyes.getCurrentSphereRef().currentRotation;

    if (current != currentRotation){
        std::stringstream ss;
        ss << currentRotation << " old " << current;
        ofSetWindowTitle(ss.str());

        // z ignored
        mainEyes.getCurrentSphereRef().rotateDeg(-current.x, 1.0f, 0.0f, 0.0f);
        mainEyes.getCurrentSphereRef().rotateDeg(-current.y, 0.0f, 1.0f, 0.0f);
        mainEyes.getCurrentSphereRef().rotateDeg(currentRotation.x, 1.0f, 0.0f, 0.0f);
        mainEyes.getCurrentSphereRef().rotateDeg(currentRotation.y, 0.0f, 1.0f, 0.0f);
        mainEyes.getCurrentSphereRef().currentRotation = currentRotation;
    }
    mainEyes.draw();
}
void SuperSphere::draw() {
    eye.start();
    //ofLogNotice("SuperSphere::draw()") << getPosition();
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
bool ImageAnimator::drawText() {
    bool found = false;
    float y = (w) ? w : 1 / 3;
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
    contours.draw(w, h, r);
}
