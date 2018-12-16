#include "ofApp.h"

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

void ImageAnimator::update() {

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

    for (auto&a : creditsText) {
        a.update();
    }

    mainEyes.update();
    rotatingEyes.update();

    imagPath.update();
    rotator.update();
    contours.update();

    // control game state
    if ((int)ofGetElapsedTimef() % 20 == 0) {
        if (level >= 0 && !winnerHitCount()) {
            level = -1;
        }
        else {
            level = 0; // trigger a game every so often, or drop back from level 2 if its taking too long
        }
    }

    if (!isAnimating()) {
        if (isWinner()) { 
            clear();
            if (++level > 1) {
                //credits will call fireworks when done
                sendFireworks = true;
                credits();
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

