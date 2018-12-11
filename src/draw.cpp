#include "ofApp.h"

void ImageAnimator::draw() {

    ofPushMatrix();
    ofPushStyle();

    // show spirl eye first, if its not running try to text if al else fails show the main eye
    if (rotatingEyes.isAnimating()) {
        rotatingEyes.draw();
    }
    else if (!drawText()) { // draw in camera
        rotate(currentRotation); // rotate screen, not object, so same rotation works every time
        mainEyes.draw();
    }

    ofPopStyle();
    ofPopMatrix();
}

void Eyes::draw() {
    if (isAnimating()) {
        for (auto& eye : eyes) { // keep all eyes in sync to make it easier
            glm::vec3 pos = eye.getPosition();
            pos.z = animator.val();
            eye.setPosition(pos);
            if (rotate) {
                eye.rotateDeg(rotate, 0.0f, 0.0f, 1.0f); //bugbug animate and menu degree bugbug make a point rotate x,y,z?
            }
        }
    }
    getCurrentEyeRef().draw();
}

void ContoursBuilder::draw(float cxScreen, float cyScreen) {
    ofPushStyle();
    ofPushMatrix();
    ofNoFill();
    ofSetLineWidth(1);// ofRandom(1, 5));
    for (auto& blob : contourDrawer.blobs) {
        ofPolyline line;
        for (int i = 0; i < blob.nPts; i++) {
            line.addVertex((imgWidth - blob.pts[i].x), blob.pts[i].y);
        }
        line.close();
        line.scale(cxScreen / imgWidth, cyScreen / imgHeight);
        line.draw();
    }
    ofSetLineWidth(5);// ofRandom(1, 5));
    if (contourFinder.blobs.size() > 0) {
        for (auto& blob : contourFinder.blobs) {
            ofPolyline line;
            for (int i = 0; i < blob.nPts; i++) {
                line.addVertex((imgWidth - blob.pts[i].x), blob.pts[i].y);
            }
            line.close();
            line.scale(cxScreen / imgWidth, cyScreen / imgHeight);
            line.draw();
            //ofDrawRectangle(blob.boundingRect.x, blob.boundingRect.y, blob.boundingRect.width, blob.boundingRect.height);
        }
    }
    ofPopMatrix();
    ofPopStyle();
}


// see if anything is going on
bool ImageAnimator::othersAreDrawing() {
    if (!rotatingEyes.isAnimating()) {
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
void ImageAnimator::drawContours(float cxScreen, float cyScreen) {
    // only draw contours if nothing else important is being drawn
    if (!othersAreDrawing()) {

        ofSetBackgroundColor(ofColor::black);
        ofSetColor(ofColor::white);
        contours.draw(cxScreen, cyScreen);
        int c = count();

        std::stringstream ss;
        ss << c << ":" << winnerCount();
        ofSetWindowTitle(ss.str());

        if (isWinner(c)) {
            // clear game then enable fireworks!!
            reset();
            credits();
        }
        else {
            // else draw boxes but only when its game 
            //if (count() >= firstMatchCount()) {
                for (auto& item : thingsToDo) {
                    item.second.draw(175);//bugbug make menu
                }
           // }
        }
    }

}
