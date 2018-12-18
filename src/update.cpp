#include "ofApp.h"

void SuperSphere::update() {
}

void Sound::update() {
    frequency = ofLerp(frequency, frequencyTarget, 0.4);

    // "lastBuffer" is shared between update() and audioOut(), which are called
    // on two different threads. This lock makes sure we don't use lastBuffer
    // from both threads simultaneously (see the corresponding lock in audioOut())
    unique_lock<mutex> lock(audioMutex);
    rms = lastBuffer.getRMSAmplitude();

}
void Eyes::update() {
    for (SuperSphere&eye : eyes) {
        eye.update();
    }
    getAnimator().update(1.0f / ofGetTargetFrameRate());
    selector.update(1.0f / ofGetTargetFrameRate());
}

void GameItem::update() {
    animater.update(1.0f / ofGetTargetFrameRate());
    if (level > 0) {
        glm::vec3 newPos= box.getPosition();
        newPos.z += box.getWidth()*animater.val()/3;
        box.setPosition(newPos);
    }
    else {
        sphere.rotateDeg(20.0f*animater.val(), 0.0f, 1.0f, 0.0f);
    }
}
bool secondsPassed(int val) {
    return ((int)ofGetElapsedTimef() % val) == 0;
}
void ImageAnimator::update() {

    if (schoolOfRock.size() != 0) {
        sound.frequencyTarget = schoolOfRock.back().frequency;
        sound.volume = schoolOfRock.back().volume;
        schoolOfRock.pop_back();
    }
    
    sound.update();

    // blinker always moving but only drawn up request
    blinker.update(1.0f / ofGetTargetFrameRate());
    if (!blinker.isOrWillBeAnimating()) {
        blinker.reset(0.0f);
        blinker.animateToAfterDelay(1.0f, ofRandom(10.0f, 30.0f)); // blink every few seconds bugbug menu
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
    if (secondsPassed((int)ofRandom(5,6))) { // start a game every minute or so bugbug set once working
        if (level < 0) {
            level = 0;
        }
    }
    if
        (secondsPassed(30)) { // if no activity reset game after 30 seconds
        if (level >= 0 && !winnerHitCount()) {
            level = -1;
        }
    }
    if (secondsPassed((int)ofRandom(45, 60*3)) && displayText.size() == 0) { // say something now and then
        credits();
    }

    if (!isAnimating()) {
        if (isWinner()) { 
            clear();
            if (++level > 1) {
                //credits will call fireworks when done
                sendFireworks = true;
                credits(true);
                level = 0; // go to first level
            }
            else {
                clear();
                level = 1; // go to next level
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

