#include "ofApp.h"

void SuperSphere::update() {
}

void Music::update() {

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

    switch (level) {
    case NoGame:
        break;
    case Basic:
        sphere.rotateDeg(20.0f*animater.val(), 0.0f, 1.0f, 0.0f);
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
}
bool secondsPassed(int val) {
    return ((int)ofGetElapsedTimef() % val) == 0;
}
void ImageAnimator::update() {

    music.update();

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
    contours.update();

    music.setPixels(contours.contourFinder);

    // control game state
    if (secondsPassed((int)ofRandom(5,6))) { // start a game every minute or so bugbug set once working
        if (level == NoGame) {
            level = Basic;
        }
    }
    if
        (secondsPassed(30)) { // if no activity reset game after 30 seconds
        if (level > NoGame && !winnerHitCount()) {
            level = NoGame;
        }
    }
    if (secondsPassed((int)ofRandom(75, 60*3)) && displayText.size() == 0) { // say something now and then
        credits();
    }
    if (!isAnimating()) {
        if (level != NoGame &&  isWinner()) {  //bugbug
            clear();
            switch (level) {
            case Basic:
                level = Medium; // go to next level
                break;
            case Medium:
                level = Difficult; // go to next level
                break;
            case Difficult:
                sendFireworks = true;
                credits(true);
                level = EndGame; // go to next level
                break;
            case EndGame:
                level = NoGame; // go to next level
                break;
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

