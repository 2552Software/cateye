#include "ofApp.h"

void ImageAnimator::draw() {

    rotate(currentRotation); // rotate screen, not object, so same rotation works every time
    mainEyes.draw();

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
            //ofDrawRectangle(blob.boundingRect.x, blob.boundingRect.y, blob.boundingRect.width, blob.boundingRect.height);
        }
    }
}


// see if anything is going on
bool ImageAnimator::isAnimating() {
    if (!rotatingEyes.isAnimating() && !mainEyes.isAnimating()) {
        for (auto& credit : creditsText) {
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
    for (auto& credit : creditsText) {
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
    ofColor c = color.getCurrentColor();
    c.a = alpha;// alpha; keep it light
    ofSetColor(c);
    myeye.start();
    box.draw();
    //sphere.drawWireframe();
    myeye.stop();
}

void ImageAnimator::drawGame() {
    ofPushStyle();
    ofPushMatrix();
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
