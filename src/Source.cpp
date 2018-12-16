#include "ofApp.h"

void Eyes::add(const std::string &name, const std::string &root, bool blink) {
    std::string blinkPath;
    if (blink) {
        std::string::size_type pos = root.find('.');
        if (pos != std::string::npos) {
            blinkPath = root.substr(0, pos);
        }
        else {
            blinkPath = root;
        }
    }
    eyes.push_back(SuperSphere());
    eyes[eyes.size() - 1].setup(root, blinkPath);
}

void Eyes::resize(int w, int h) {
    float r = std::min(w, h);
    for (SuperSphere&eye : eyes) {
        eye.setRadius(r);
        eye.setPosition((w / 2) - r, (h / 2) - (r / 2), 0);
    }
}

void  ImageAnimator::fireWorks() {
    sounds(5);
   rotatingEyes.getAnimator().animateFromTo(-300, 300);//bugbug will need to adjsut for pi
}

void GameItem::trigger() {
    if (!isAnimating()) {
        float duration = 25.0f;//seconds bugbug make menu
        ofColor c1(ofColor::red);// (0.0f, 0.0f, ofRandom(200, 255));
        ofColor c2(ofColor::blue);//ofRandom(200, 255), 0.0f, 0.0f);
        color.setColor(c1);
        color.setDuration(duration);
        color.setRepeatType(PLAY_ONCE);
        color.setCurve(LINEAR);
        color.animateTo(c2);
    }
}

void ImageAnimator::rotate(const ofVec3f& target) {
    std::stringstream ss;
    ss << target;
    //ofSetWindowTitle(ss.str());
    if (target.x || target.y || target.z) {
        //ofLogNotice() << "rotate to targert" << target;
        ofRotateDeg(target.x, 1.0f, 0.0f, 0.0f);
        ofRotateDeg(target.y, 0.0f, 1.0f, 0.0f);
        ofRotateDeg(target.z, 0.0f, 0.0f, 1.0f);
    }
}

void ImageAnimator::sounds(int duration) {
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(mySounds), std::end(mySounds), rng);

    for (ofSoundPlayer&player : mySounds) {
        player.setVolume(ofRandom(2.2f));
        player.setPosition(ofRandom(1.0f));
        player.setMultiPlay(true);
        int end = (int)ofRandom(1, duration);
        for (int i = 0; i < end; ++i) {
            player.play();
        }
        // restore for next time
        player.setMultiPlay(false);
        player.setPosition(0);
        player.setVolume(1.0);
    }
}

// turn on/off came items
void ImageAnimator::clear() {
    gameItems.clear();
    level = 0;
}
// put the first few items in that must before the entire game is unlocked
void ImageAnimator::randomize() {
    clear(); // reset 
    // make sure we get 3 or random points used to unlock the game
    for (size_t c = 0; c < firstMatchCount(); ) {
        int i = (int)ofRandom(10, cameraMapping.size() - 11); // keep from the edges
        int index = 0;
        for (auto& item : cameraMapping) {
            if (index == i) {
                float cx = ofGetScreenWidth()- (item.second.width)*xFactor;/// ofGetScreenWidth();
                gameItems.push_back(GameItem(ofRectangle((cx-item.second.x*xFactor), item.second.y*yFactor, item.second.width*xFactor, item.second.height*yFactor), mainEyes.getCurrentEyeRef().getMainEye()));
                ++c;
                break;
            }
            ++index;
        }
    }
}

void ImageAnimator::setTriggerCount(float count) {
    if (count > 0) {
        maxForTrigger = count;
    }
}

void ImageAnimator::setCount(int count) {
    if (count > 0) {
        squareCount = count;
    }
}
ImageAnimator::ImageAnimator() {
    maxForTrigger = 525.0f;
    shapeMinSize = 200.0f; // menus bugbug
    squareCount = 10;// menus bugbu
}

// count of items selected
size_t ImageAnimator::winnerHitCount() {
    return gameItems.size();
}

void ImageAnimator::reset() {
    sendFireworks = false;
    buildTable();
    randomize();
}

void ImageAnimator::creditsDone(TextEvent & event) {
    if (sendFireworks) {
        sendFireworks = false;
        fireWorks();
    }
}

void ImageAnimator::rotatingEyesDone(ofxAnimatableFloat::AnimationEvent & event) {
    // no move main eye back into focus
    currentRotation.set(0.0f, 0.0f); // look forward, move ahead its not too late
    mainEyes.getAnimator().animateFromTo(-rotatingEyes.getCurrentEyeRef().getRadius(), 0.0f);
    randomize(); // reset and start again
}

void ImageAnimator::windowResized(int w, int h) {
    // convert to screen size
    xFactor = w / cameraWidth;
    yFactor = h / cameraHeight;

    mainEyes.resize(w,h);
    rotatingEyes.resize(w, h);

    clear(); // reset game to assure all sizes are correct
    
}

void ImageAnimator::startPlaying() {
    sounds();
}
