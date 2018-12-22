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
void Game::updateLevel() {

    switch (gameLevel) {
    case NoGame:
        if (getLevelDuration() > 60.0f) { // start game every 60 seconds bugbug 5 sec to test
            resetLevelTime();
            gameLevel = Basic; // go to next level
        }
        break;
    case Basic:
        if (getLevelDuration() > 60.0f) { // stop game after 1 minute at a level
            resetLevelTime();
            gameLevel = NoGame; // go to previous level
        }
        break;
    case Medium:
        if (getLevelDuration() > 60.0f) { // stop game after 1 minute at a level
            resetLevelTime();
            gameLevel = Basic; 
        }
        break;
    case Difficult:
        if (getLevelDuration() > 60.0f) { // stop game after 1 minute at a level
            resetLevelTime();
            gameLevel = Medium;
        }
        break; 
    case EndGame:
        if (getLevelDuration() > 60.0f) { // start game every 60 seconds
            resetLevelTime();
            gameLevel = NoGame; // go to next level
        }
        break;
    }

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

    fancyText.update();
    basicText.update();

    mainEyes.update();
    rotatingEyes.update();
    contours.update();

    updateLevel();

    if (!isAnimating()) {
        if (gameLevel != NoGame &&  isWinner()) {  
            clear();
            switch (gameLevel) {
            case Basic:
                gameLevel = Medium; // go to next level
                break;
            case Medium:
                gameLevel = Difficult; // go to next level
                break;
            case Difficult:
                sendFireworks = true;
                credits(true);
                gameLevel = EndGame; // go to next level
                break;
            case EndGame:
                gameLevel = NoGame; // go to next level
                break;
            }
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

