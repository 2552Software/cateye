#include "ofApp.h"

void ImageAnimator::draw() {

    ofPushMatrix();
    ofPushStyle();

    // show spirl eye first, if its not running try to text if al else fails show the main eye
    if (rotatingEyeZ.isAnimating()) {
        spirl.bind();
        glm::vec3 pos = sphere4Spirl.getPosition();
        pos.z = rotatingEyeZ.val();
        sphere4Spirl.setPosition(pos);
        sphere4Spirl.rotateDeg(20.0f, 0.0f, 0.0f, 1.0f); //bugbug animate and menu
        sphere4Spirl.draw();
        spirl.unbind();
    }
    else if (!drawText()) { // draw in camera
        if (mainEyeZ.isAnimating()) {
            for (auto& eye : eyes) {
                glm::vec3 pos = eye.getPosition();
                pos.z = mainEyeZ.val();
                eye.setPosition(pos);
            }
        }
        rotate(currentRotation); // rotate screen, not object, so same rotation works every time
        getCurrentEyeRef().draw();
    }

    ofPopStyle();
    ofPopMatrix();

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
    if (!rotatingEyeZ.isAnimating()) {
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
            if (count() >= firstMatchCount()) {
                for (auto& item : thingsToDo) {
                    item.second.draw(175);//bugbug make menu
                }
            }
        }
    }

}
