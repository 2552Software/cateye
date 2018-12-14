#include "ofApp.h"

void ImageAnimator::draw() {

    //if (drawText()) { // draw text first, if no text draw the eye
   //     return;
   // }

    // show spirl eye first, if its not running try to text if al else fails show the main eye
  //  if (rotatingEyes.isAnimating()) {
     //   rotatingEyes.draw();
       // return;
  //  }

   /// ofPushMatrix();
   // rotate(currentRotation); // rotate screen, not object, so same rotation works every time
    //mainEyes.draw();
   // ofPopMatrix();

    drawGame(); // draw any game that may be running

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
void GameItem::draw(int alpha, float xFactor, float yFactor) {
    if (isAnimating()) {
        ofPushStyle();
        ofNoFill();
        ofEnableAlphaBlending();
        ofColor c = color.getCurrentColor();
        c.a = alpha;// alpha; keep it light
        ofSetColor(c);
        ofDrawRectangle(xFactor*rectangle.x, yFactor*rectangle.y, 0.0f,xFactor*rectangle.width, yFactor*rectangle.height);
        ofBoxPrimitive box;
        box.set(xFactor*rectangle.width, yFactor*rectangle.height, 0.0f);
        box.setPosition(xFactor*rectangle.x, yFactor*rectangle.y, 0.0f);
        //box.rollDeg(10.0f);
       // box.drawWireframe();
        ofDisableAlphaBlending();
        ofPopStyle();
    }
    else {
        ofPushStyle();
        ofFill();
        ofEnableAlphaBlending();
        ofColor c = color.getCurrentColor();
        c.a = alpha;// alpha; keep it light
        ofSetColor(c);
        ofDrawRectangle(xFactor*rectangle.x, yFactor*rectangle.y, xFactor*rectangle.width, yFactor*rectangle.height);
        ofBoxPrimitive box;
        box.set(xFactor*rectangle.width, yFactor*rectangle.height, 0.0f);
        box.setPosition(xFactor*rectangle.x, yFactor*rectangle.y, 0.0f);
        //box.rollDeg(10.0f);
       // box.drawWireframe();
        ofDisableAlphaBlending();
        ofPopStyle();
    }
}

void ImageAnimator::drawGame() {
    // convert camera mapping to screen mapping and draw
    //if (count() >= firstMatchCount()) {
    float screenW = cameraWidth * xFactor;
    float gameW = xGameItems * xFactor*gameItemWidth;
    float moveX = (screenW - gameW) / 2;
    //ofTranslate(ofGetWidth()/2, ofGetHeight() / 3);
    ofTranslate(-gameW, 0.0f);
    for (auto& item : cameraMapping) {
        item.second.draw(175, xFactor, yFactor);//bugbug make menu
    }
    //}
}

void ImageAnimator::drawContours(float cxScreen, float cyScreen) {
    contours.draw(cxScreen, cyScreen);
}
