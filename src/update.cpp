#include "ofApp.h"

void SuperSphere::update() {
}

void Eyes::update() {
    for (SuperSphere&eye : eyes) {
        eye.update();
    }
    getAnimator().update(1.0f / ofGetTargetFrameRate());
    selector.update(1.0f / ofGetTargetFrameRate());
}

void GameItem::update() {
    color.update(1.0f / ofGetTargetFrameRate());
    sphere.tiltDeg(5.0f);
}
bool secondsPassed(int val) {
    return ((int)ofGetElapsedTimef() % val) == 0;
}
void ImageAnimator::update() {

    // blinker always moving but only drawn up request
    blinker.update(1.0f / ofGetTargetFrameRate());
    if (!blinker.isOrWillBeAnimating()) {
        blinker.reset(0.0f);
        blinker.animateToAfterDelay(1.0f, ofRandom(1.0f, 10.0f)); // blink every few seconds bugbug menu
    }

    // if not animating time to go...
    std::list<GameItem>::iterator i = gameItems.begin();
    while (i != gameItems.end())   {
        if (!(*i).isAnimating())    {
            i = gameItems.erase(i); 
        }
        else {
            ++i;
        }
    }

    for (auto& a : gameItems) {
        a.update();
    }

    for (auto&a : displayText) {
        a.update();
    }

    mainEyes.update();
    rotatingEyes.update();

    imagPath.update();
    rotator.update();
    contours.update();

    // control game state
    if (secondsPassed(60)) { // start a game every minute
        if (level < 0) {
            level = 0;
        }
    }
    if (secondsPassed(20)) { // if no activity reset game after 20 seconds
        if (level >= 0 && !winnerHitCount()) {
            level = -1;
        }
    }
    if (secondsPassed((int)ofRandom(45, 60*3))) { // say something now and then
        credits();
    }

    if (!isAnimating()) {
        if (isWinner()) { 
            clear();
            if (++level > 1) {
                //credits will call fireworks when done
                sendFireworks = true;
                credits(true);
            }
        }
        else {
            getCountours();
        }
    }
}

// return true if updated
void ContoursBuilder::update() {
    video.update();
    if (video.isFrameNew()) { // && (ofGetFrameNum() & 1) to slow things down
                              // clear less often
        colorImg.setFromPixels(video.getPixels());
        grayImage = colorImg; // convert our color image to a grayscale image
        if (backgroundImage.bAllocated) {
            grayDiff.absDiff(backgroundImage, grayImage);
        }
        backgroundImage = grayImage; // only track new items -- so eye moves when objects move
        grayDiff.threshold(50); // turn any pixels above 50 white, and below 100 black bugbug menu can tune game here too
        if (!contourFinder.findContours(grayDiff, 5, (cameraWidth*cameraHeight), 128, false, true)) {
            contourFinder.blobs.clear(); // removes echo but does it make things draw too fast?
        }
        grayImage.blurGaussian(3);
        grayImage.threshold(50);
        if (!contourDrawer.findContours(grayImage, 5, (cameraWidth*cameraHeight), 128, true)) {
            contourDrawer.blobs.clear();
        }
    }
}

