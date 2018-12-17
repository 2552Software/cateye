#include "ofApp.h"

void ImageAnimator::setTitle() {
    std::stringstream ss;
    ss << winnerHitCount() << " of " << winnerThreshold();
    std::string s = "Game On! Find ";
    s += ss.str();
    ofSetColor(ofColor::white);
    font.drawStringAsShapes(s, ofGetScreenWidth() / 2 - font.stringWidth(s) / 2, font.getLineHeight() * 5);

}
void ImageAnimator::blink() {
    // blink upon request
    ofSetColor(ofColor::black);
    ofPushStyle();
    ofFill();
    ofDrawRectangle(0, 0, ofGetScreenWidth(), (ofGetScreenHeight() / 2)*blinker.val());
    ofDrawRectangle(0, ofGetScreenHeight(), ofGetScreenWidth(), -(ofGetScreenHeight() / 2)*blinker.val());
    ofPopStyle();
}

void Eyes::add(const std::string &name, const std::string &root) {
    eyes.push_back(SuperSphere(root));
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
    sendFireworks = false;
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
    clear(); // reset and start again
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

void ImageAnimator::getCountours() {
    float max = 0.0f;
    if (contours.contourFinder.blobs.size() > 0) {
        glm::vec3 target = currentRotation;
        glm::vec3 centroid;
        ofRectangle rect;

        // first find any motion for the game, then find motion for drawing and eye tracking
        for (auto& blob : contours.contourFinder.blobs) {
            if (blob.area > max && blob.boundingRect.x > 1 && blob.boundingRect.y > 1) {  //x,y 1,1 is some sort of strange case
                max = blob.area;
                centroid = blob.centroid;
                rect = blob.boundingRect;
                break; // first is max
            }
        }

        // see if we have a trigger and we are not at level -1 which means no game
        if (level >= 0) {
            for (auto& blob : contours.contourFinder.blobs) {
                if (blob.area > maxForTrigger && blob.boundingRect.x > 1 && blob.boundingRect.y > 1) {  //x,y 1,1 is some sort of strange case
                    if (blob.area >= maxForTrigger) {
                        // see if we can trigger with this one
                        for (auto& item : cameraMapping) { // get all blocks within region
                            if (item.second.inside(blob.boundingRect)) { //
                                float cx = ofGetScreenWidth() - (item.second.width)*xFactor;/// ofGetScreenWidth();
                                ofRectangle rect2Use((cx - item.second.x*xFactor), item.second.y*yFactor, item.second.width*xFactor, item.second.height*yFactor);
                                if (!find(rect2Use)) {
                                    if (ofRandom(10.0f) > 5.0) {
                                        frequency += ofRandom(50.0f);
                                    }
                                    else {
                                        frequency -= ofRandom(50.0f);
                                    }
                                    if (frequency < 10.0f) {
                                        frequency = 20.0f;
                                    }
                                    else if (frequency > 320.0f) {
                                        frequency = 300.0f;
                                    }
                                    std::string name = DATAPATH;
                                    if (level > 1) {
                                        name += "\\cube\\cube.jpg";
                                        gameItems.push_back(GameItem(rect2Use, Eye(name), level));
                                    }
                                    else {
                                        name += "\\circle\\eye1.jpg";
                                        gameItems.push_back(GameItem(rect2Use, Eye(name), level));
                                       // gameItems.push_back(GameItem(rect2Use, mainEyes.getCurrentEyeRef().getMainEye(), level));
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (max > shapeMinSize) { // fine tune on site 
            int w = cameraWidth; // camera size not screen size
            int h = cameraHeight;

            double x = (centroid.x / cameraWidth)*100.0f; // make it a percent
            double y = (centroid.y / cameraHeight)*100.0f; // make it a percent

            //if (mapX.find(std::make_pair(xAction, yAction)) != mapX.end()) {
           // }
            for (auto& row : mapCameraInX) {
                if (x >= row.first.first && x <= row.first.second) {
                    target.y = row.second;
                    // centroid
                    break;
                }
            }
            for (auto& row : mapCameraInY) {
                if (y >= row.first.first && y <= row.first.second) {
                    target.x = row.second;
                    break;
                }
            }
        }
        // if any data 
        if (max > 15) {
            //   ofLogNotice() << "insert targert" << target;
            currentRotation = target;
        }
        else {
            // no new data so home the eye (?should we add a time?)
            currentRotation.set(0.0L, 0.0L, 0.0L);
        }
    }
}