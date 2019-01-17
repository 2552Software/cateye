#include "ofApp.h"

void SuperSphere::update() {
    rotater.update(1.0f / ofGetTargetFrameRate());
    Animate3d::update();
    if (seconds > 0.0f && !isRunning()) {
        stop(); 
    }
}
void Animate3d::update() {
    animatorUp.update(1.0f / ofGetTargetFrameRate());
    animatorDown.update(1.0f / ofGetTargetFrameRate());
};

void TextTimer::update() {
    int elapsed = ofGetSystemTimeMillis() - timeBegan;
    if (timeDelay) {
        if (timeDelay < elapsed) {
            timeBegan = ofGetSystemTimeMillis(); // here we go
            elapsed = ofGetSystemTimeMillis() - timeBegan; // time to start
            timeDelay = 0.0; // delay is gone
        }
        else {
            return;
        }
    }

    if (elapsed > timeToRender+lingerTime || !rawText.size() || doneDrawing) {
        doneDrawing = true;
        partialText = rawText;
        return;
    }
    float factor = elapsed / timeToRender;  // ratio of seconds that passed to our full range of time, say 20% or 0.2

    size_t n = (int)(factor * rawText.length());
    if (n > rawText.length()) {
        partialText = rawText;
        //doneDrawing = true;
    }
    else {
        partialText = rawText.substr(0, n);
    }
}

void Textures::update() {
    selector.update(1.0f / ofGetTargetFrameRate());
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
    fullScreenText.erase(std::remove_if(fullScreenText.begin(),
        fullScreenText.end(),
        [](const TextTimer& item) {
            return item.doneDrawing;
        }),   fullScreenText.end());

    inlineText.erase(std::remove_if(inlineText.begin(),
        inlineText.end(),
        [](const TextTimer& item) {
        return item.doneDrawing;
    }),    inlineText.end());

}
void Blinker::update() {
    blinker.update(1.0f / ofGetTargetFrameRate());
    if (!blinker.isOrWillBeAnimating()) {
        blinker.reset(0.0f);
        blinker.animateToAfterDelay(1.0f, ofRandom(10.0f, 30.0f)); // blink every few seconds bugbug menu
    }
}
void Game::update() {

    // blinker always moving but only drawn up request
    blinker.update();
    for (auto a : gameEyes) {
        a.update();
    }
    gameEyes.erase(std::remove_if(gameEyes.begin(),
        gameEyes.end(),
        [](EyeGameItem item) {
        return !item.isRunning();
    }), gameEyes.end());

    fancyText.update(); //bugbug put a GET LOUND in here
    basicText.update();
    mainEye.update();
    rotatingEye.update();
    contours.update();
    mainEyesSkins.update();
    rotatingEyesSkins.update();
    cubesSkins.update();
    spheresSkins.update();
    cylindersSkins.update();
    musicNotesSkins.update();

    if (current.inGame() && isWinner()) {  
        if (isWinner()) {
            clear();
            switch (current.getLevel()) {
            case GameLevel::Difficult:
                sendFireworks = true;
                credits(true);
                break;
            default:
                break;
            }
            current.next();
        }
        if (current.advance()) {
            gameEyes.clear();
        }
    }
    else {
        if (ofRandom(10.0f) > 8.0f && !fancyText.isFullScreenAnimating()) {
            credits(false); // funny text or maybe credits
        }
    }
    getCountours();
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

