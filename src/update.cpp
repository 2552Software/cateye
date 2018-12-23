#include "ofApp.h"

void TextTimer::update() {
    int elapsedMilliSeconds = ofGetSystemTimeMillis() - timeBegan;
    if (timeDelay) {
        if (timeDelay < elapsedMilliSeconds) {
            timeBegan = ofGetSystemTimeMillis(); // here we go
            elapsedMilliSeconds = ofGetSystemTimeMillis() - timeBegan; // time to start
            timeDelay = 0.0; // delay is gone
        }
        else {
            return;
        }
    }

    if (elapsedMilliSeconds > timeToRender || !rawText.size() || doneDrawing) {
        doneDrawing = true;
        partialText = rawText;
        return;
    }
    float factor = elapsedMilliSeconds / timeToRender;  // ratio of seconds that passed to our full range of time, say 20% or 0.2

    size_t n = (int)(factor * rawText.length());
    if (n > rawText.length()) {
        partialText = rawText;
        doneDrawing = true;
    }
    else {
        partialText = rawText.substr(0, n);
    }
}
void SuperSphere::update() {
    animator.update(1.0f / ofGetTargetFrameRate());
}
void SuperCube::update() {
    animator.update(1.0f / ofGetTargetFrameRate());
}
void Textures::update() {
    selector.update(1.0f / ofGetTargetFrameRate());
}
void SuperCylinder::update() {
    animator.update(1.0f / ofGetTargetFrameRate());
}

void CylinderGameItem::update() {
    cylinder.update();
    if (!cylinder.isAnimating()) {
        stop();
    }
}

void CubeGameItem::update() {
    cube.update();
    if (!cube.isAnimating()) {
        stop();
    }
}
void SphereGameItem::update() {
    sphere.update();
    if (!sphere.isAnimating()) {
        stop();
    }
    sphere.rotateDeg(20.0f*sphere.getAnimator().val(), glm::vec3(0.0f, 1.0f, 0.0f));
    sphere.rotateAroundDeg(15.0f*sphere.getAnimator().val(), glm::vec3(0.0f, 1.0f, 0.0f), sphere.getParent()->getPosition());
    //sphere.orbitDeg(15.0f*sphere.getAnimator().val(), 0.0f, getRadiusGlobal(), *sphere.getParent());
/*
    switch (level) {
    case NoGame:
        break;
    case Basic: {
        sphere.rotateDeg(20.0f*animater.val(), glm::vec3(0.0f, 1.0f, 0.0f));
    }
        break;
    case Medium: {
        glm::vec3 newPos = box.getPosition();
        newPos.z += box.getWidth()*animater.val() / 3;
        box.setPosition(newPos);
        }
        break;
    case Difficult:
        cylinder.rotateDeg(20.0f*animater.val(), 0.0f, 1.0f, 0.0f);
        break;
    case EndGame:
        break;
    }
*/
}
bool secondsPassed(int val) {
    return ((int)ofGetElapsedTimef() % val) == 0;
}

void TextEngine::update() {
    for (auto&a : fullScreenText) {
        a.update();
    }
    for (auto&a : inlineText) {
        a.update();
    }
    // remove all that are done
    fullScreenText.remove_if(TextTimer::isReadyToRemove);
    inlineText.remove_if(TextTimer::isReadyToRemove);
}
void Game::update(Music*music) {

    // blinker always moving but only drawn up request
    blinker.update(1.0f / ofGetTargetFrameRate());
    if (!blinker.isOrWillBeAnimating()) {
        blinker.reset(0.0f);
        blinker.animateToAfterDelay(1.0f, ofRandom(10.0f, 30.0f)); // blink every few seconds bugbug menu
    }

    // if not animating time to go...
    for (auto& a : gameItems) {
        a->update();
    }
    gameItems.remove_if(GameItem::isReadyToRemove);

    if (current->getLevelDuration() > current->duration()) { // start game every 60 seconds bugbug 5 sec to test
        current = current->getNext();
    }

    fancyText.update();
    basicText.update();

    mainEye.update();
    rotatingEye.update();
    contours.update();

    if (!isAnimating()) {
        if (current->level() != NoGame &&  isWinner()) {  
            clear();
            switch (current->level()) {
            case Difficult:
                sendFireworks = true;
                credits(true);
                break;
            }
            current = current->getNext();
        }
        else {
            getCountours(music);
        }
    }
}

// return true if updated
void ContoursBuilder::update() {
    video.update();
    if (video.isFrameNew()) { // && (ofGetFrameNum() & 1) to slow things down

        colorImg.setFromPixels(video.getPixels());

        grayImage = colorImg; // convert our color image to a grayscale image

        if (backgroundImage.bAllocated) {
            grayDiff.absDiff(backgroundImage, grayImage);
        }

        backgroundImage = grayImage; // only track new items -- so eye moves when objects move

        grayDiff.threshold(30); // turn any pixels above 30 white, and below 100 black bugbug menu can tune game here too
        if (!contourFinder.findContours(grayDiff, 5, (cameraWidth*cameraHeight), 255, false, true)) {
            contourFinder.blobs.clear(); // removes echo but does it make things draw too fast?
        }
        grayImage.blurGaussian(3);
        grayImage.threshold(50);
        if (!contourDrawer.findContours(grayImage, 5, (cameraWidth*cameraHeight), 255, false)) {
            contourDrawer.blobs.clear();
        }

    }
}

