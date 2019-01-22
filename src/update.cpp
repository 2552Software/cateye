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
    if (timeDelay < ++elapsed) {
        finalText = rawText;
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
            return item.completed();
        }),   fullScreenText.end());

    inlineText.erase(std::remove_if(inlineText.begin(),
        inlineText.end(),
        [](const TextTimer& item) {
        return item.completed();
    }),    inlineText.end());
}
void Blinker::update() {
    blinker.update(1.0f / ofGetTargetFrameRate());
    if (!blinker.isOrWillBeAnimating()) {
        blinker.reset(0.0f);
        blinker.animateToAfterDelay(1.0f, ofRandom(10.0f, 30.0f)); //bugbug make range wider blink every few seconds bugbug menu
    }
}
void Game::nextLevel() {
    if (current.getLevel() == GameLevel::Basic) {
        playSound("war_games_play_a_game.wav");
        ofSetColor(ofColor::hotPink);
    }
    else if (current.getLevel() == GameLevel::Medium) {
        playMoreRandomSound("breakingglass.wav");
        ofSetColor(ofColor::orangeRed);
    }
    else if (current.getLevel() == GameLevel::Difficult) {
        playSound("accessgranted.wav");
        ofSetColor(ofColor::yellowGreen);
    }
    else {
        playMoreRandomSound("fanfare.wav");
        ofSetColor(ofColor::white);
    }
    gameEyes.clear();

}
void Game::update() {

    // blinker always moving but only drawn up request
    blinker.update();
    for (auto a : gameEyes) {
        a.update();
    }
    gameEyes.erase(std::remove_if(gameEyes.begin(),
        gameEyes.end(),
        [](EyeGameItem& item) {
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
    current.update();
    if (current.checkTimeOut()) {
        gameEyes.clear();
        nextLevel();
    }

    if (current.inGame() && isWinner()) {  
        clear();
        switch (current.getLevel()) {
        case GameLevel::Difficult:
            sendFireworks = true;
            credits(true);
            break;
        default:
            playMoreRandomSound("cat.mp3");
            current.next();
            nextLevel();
            break;
        }
    }
    else if (!current.inGame() && !fancyText.isFullScreenAnimating()) {
        if (ofRandom(100.0f) > 99.9f) {
            playMoreRandomSound("neon_light.wav");
            credits(false); // funny text or maybe credits
        }
        else if (ofRandom(1000.0f) > 999.9f) {
            playMoreRandomSound("honk_honk_x.wav");
        }
        else if (ofRandom(100.0f) > 99.9f) {
            playMoreRandomSound("jaw_harp.wav");
        }
        else if (ofRandom(1000.0f) > 999.9f) {
            playMoreRandomSound("pacman_x.wav");
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

        grayDiff.threshold(70, true); // turn any pixels above 30 white, and below 100 black bugbug menu can tune game here too
        if (!contourFinder.findContours(grayDiff, 5, (cameraWidth*cameraHeight), 255, true, true)) {
            contourFinder.blobs.clear(); // removes echo but does it make things draw too fast?
        }
        grayDraw = grayImage;
        grayDraw.mirror(false, true);
        grayImage.blurGaussian();
        grayImage.threshold(80, true);
        if (!contourDrawer.findContours(grayImage, 5, (cameraWidth*cameraHeight), 255, true)) {
            contourDrawer.blobs.clear();
        }

    }
}

