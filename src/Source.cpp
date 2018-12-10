#include "ofApp.h"


void  ImageAnimator::fireWorks() {
    sounds(5);
    ignight(false);
    spirlRadius.animateFromTo(sphere4Spirl.getRadius()- sphere4Spirl.getRadius()/10, getCurrentEyeRef().getRadius() + -sphere4Spirl.getRadius() / 3);//bugbug change with screen size
    spirlRadius.animateToIfFinished(sphere4Spirl.getRadius() / 5);
}

void Map::trigger() {
    if (!isAnimating()) {
        float duration = 25.0f;//seconds bugbug make menu
        game.reset(5.0f);
        game.setCurve(LINEAR);
        game.setRepeatType(PLAY_ONCE);
        game.setDuration(duration);
        game.animateTo(255.0f);
        ofColor c1(ofColor::orange);// (0.0f, 0.0f, ofRandom(200, 255));
        ofColor c2(ofColor::cyan);//ofRandom(200, 255), 0.0f, 0.0f);
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

void ImageAnimator::ignight(bool on) {
    // create hot grids
    for (auto& a : thingsToDo) {
        a.second.set((on) ? 1:0); // clear all is 0 menu pick -- all 1s enable all
    }
    level = (on) ? 1 : 0;
}
// call just after things are found and upon startup
void ImageAnimator::randomize() {
    ignight(false); // reset 
    // make sure we get 3 or random points used to unlock the game
    for (int c = 0; c < firstMatchCount(); ) {
        int i = (int)ofRandom(10, thingsToDo.size() - 11); // keep from the edges
        int index = 0;
        for (auto& item : thingsToDo) {
            if (index == i){
                item.second.set(1);
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
        reset();
    }
}

void ImageAnimator::setCount(int count) {
    if (count > 0) {
        squareCount = count;
        reset();
    }
}
ImageAnimator::ImageAnimator() {
    maxForTrigger = 25.0f;
    shapeMinSize = 200.0f; // menus bugbug
    squareCount = 10;// menus bugbu
    level = 0;
}


void ImageAnimator::add(const std::string &name, const std::string &root) {
    std::string::size_type pos = root.find('.');
    std::string blinkPath;
    if (pos != std::string::npos) {
        blinkPath = root.substr(0, pos);
    }
    else {
        blinkPath = root;
    }
    eyes.push_back(SuperSphere());
    eyes[eyes.size() - 1].setup(name, blinkPath);
}
// count of items selected
int ImageAnimator::count() {
    int count = 0;
    for (auto& item : thingsToDo) {
        if (item.second.isAnimating()) {
            ++count;
        }
    }
    return count;
}

void ImageAnimator::reset() {
    buildTable();
    randomize();
}
 
void ImageAnimator::creditsDone(TextEvent & event) {
    fireWorks();
}

void ImageAnimator::spirlDone(ofxAnimatableFloat::AnimationEvent & event) {
    float r = getCurrentEyeRef().getRadius();
   
    eyeRadius.animateFromTo(sphere4Spirl.getRadius() - sphere4Spirl.getRadius() / 10, r);
}

void ImageAnimator::windowResized(int w, int h) {
    for (SuperSphere&eye : eyes) {
        eye.setRadius(std::min(w, h));
    }
    sphere4Spirl.setRadius(std::min(w, h));

    // duration bugbug set in menu
    spirlRadius.reset();
    spirlRadius.setDuration(5.0f);
    spirlRadius.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    spirlRadius.setCurve(EASE_IN_EASE_OUT);

    eyeRadius.reset();
    eyeRadius.setDuration(1.0f);
    eyeRadius.setRepeatType(PLAY_ONCE);
    eyeRadius.setCurve(EASE_IN_EASE_OUT);
}

void ImageAnimator::startPlaying() {
    animatorIndex.animateTo(eyes.size() - 1);
    sounds();
}
SuperSphere&ImageAnimator::getCurrentEyeRef() {
    return eyes[(int)animatorIndex.getCurrentValue()];
}
