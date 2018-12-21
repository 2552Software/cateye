#include "ofApp.h"
#include "sound.h"

void TextEngine::print(const std::string& s) {
    ofPushMatrix();
    float stringw = font.stringWidth(s);
    ofTranslate(ofGetWidth() / 2 - stringw / 2, ofGetHeight() / 2, getRadius());
    font.drawStringAsShapes(s, 0, 0);
    ofPopMatrix();

}

// let folks know we are in a game
void Game::setTitle() {
    if (inGame()) {
        std::stringstream ss;
        ss << winnerHitCount() << " of " << winnerThreshold();
        std::string s = "Game On! Find ";
        s += ss.str();
        text.print(s);
        /* good example for later
        ofPushMatrix();
        ofTranslate(w / 2, h / 2);// , getRadius());
        // get the string as paths
        vector < ofPath > paths = font.getStringAsPoints(s);
        for (int i = 0; i < paths.size(); i++) {
            // for every character break it out to polylines
            vector <ofPolyline> polylines = paths[i].getOutline();
            // for every polyline, draw every fifth point
            for (int j = 0; j < polylines.size(); j++) {
                for (int k = 0; k < polylines[j].size(); k += 5) {         // draw every "fifth" point
                    ofDrawCircle(polylines[j][k], 3);
                }
            }
        }
        bugbug use the Text class
        ofPushMatrix();
        float stringw = font.stringWidth(s);
        ofTranslate(ofGetWidth() / 2- stringw/2, ofGetHeight() / 2, getRadius());
        font.drawStringAsShapes(s, 0, 0);
        ofPopMatrix();
        */

   }
}
void Game::blink() {
    // blink upon request
    ofSetColor(ofColor::black);
    ofPushStyle();
    ofFill();
    ofDrawRectangle(0, 0, w, (h / 2)*blinker.val(), getRadius());
    ofDrawRectangle(0, h, w, -(h / 2)*blinker.val(), getRadius());
    ofPopStyle();
}

void Eyes::add(const std::string &name, const std::string &root) {
    eyes.push_back(SuperSphere(root));
}


void  Game::fireWorks() {
   //bugbug sounds(5);
   rotatingEyes.getAnimator().animateFromTo(-300, 300);//bugbug will need to adjsut for pi
}

void Game::rotate(const ofVec3f& target) {
    //std::stringstream ss;
   // ss << target;
    //ofSetWindowTitle(ss.str());
    if (target.x || target.y || target.z) {
        //ofLogNotice() << "rotate to targert" << target;
        ofRotateDeg(target.x, 1.0f, 0.0f, 0.0f);
        ofRotateDeg(target.y, 0.0f, 1.0f, 0.0f);
        ofRotateDeg(target.z, 0.0f, 0.0f, 1.0f);
    }
}

void Game::sounds(int duration) {
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
void Game::clear() {
    gameItems.clear();
    sendFireworks = false;
}


void Game::setTriggerCount(float count) {
    if (count > 0) {
        maxForTrigger = count;
    }
}

void Game::setCount(int count) {
    if (count > 0) {
        squareCount = count;
    }
}
Game::Game() {
    maxForTrigger = 525.0f;
    shapeMinSize = 200.0f; // menus bugbug
    squareCount = 10;// menus bugbu
    w = ofGetWidth();
    h = ofGetHeight();
}

void Game::textDone(int id, bool inLine) {
    // check ID too make unique
    if (!inLine && sendFireworks) {
        sendFireworks = false;
        fireWorks();
    }

}

// count of items selected
size_t Game::winnerHitCount() {
    return gameItems.size();
}

void Game::rotatingEyesDone(ofxAnimatableFloat::AnimationEvent & event) {
    // no move main eye back into focus
    currentRotation.set(0.0f, 0.0f); // look forward, move ahead its not too late
    mainEyes.getAnimator().animateFromTo(-rotatingEyes.getCurrentSphereRef().getRadius(), 0.0f);
    clear(); // reset and start again
}

void Game::windowResized(int wIn, int hIn) {
    w = wIn;
    h = hIn;
    ofLogNotice("ofApp::setup") << "Window size " << w << " by " << h;

    // convert to screen size
    xFactor = w / cameraWidth;
    yFactor = h / cameraHeight;

    clear(); // reset game to assure all sizes are correct
    
}

void Game::startPlaying() {
   //bugbug sounds();
}
size_t Game::winnerThreshold() { 
    switch (gameLevel) {
    case NoGame:
        return 0;
    case Basic:
        return screenToAnimationMap.size() / 3;
    case Medium:
        return screenToAnimationMap.size() / 2;
    case Difficult:
        return screenToAnimationMap.size();
    case EndGame:
        return 1; // it just ages out
    }
    return 0;
}

void Game::getCountours(Music*music) {
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
        if (inGame()) {
            for (auto& blob : contours.contourFinder.blobs) {
                if (blob.area > maxForTrigger && blob.boundingRect.x > 1 && blob.boundingRect.y > 1) {  //x,y 1,1 is some sort of strange case
                    if (blob.area >= maxForTrigger) {
                        // see if we can trigger with this one
                        for (auto& item : screenToAnimationMap) { // get all blocks within region
                            if (item.second.inside(blob.boundingRect)) { //
                                float cx = w - (item.second.width)*xFactor;
                                ofRectangle rect2Use((cx - item.second.x*xFactor), item.second.y*yFactor, item.second.width*xFactor, item.second.height*yFactor);
                                if (!find(rect2Use)) {
                                    switch (gameLevel) {
                                    case Basic: // only 1/3 items saved etc
                                        if ((item.second.c) % 3 == 0) {
                                            gameItems.push_back(GameItem(rect2Use, spheres.getCurrentSphereRef().getMainEye(), gameLevel, item.second.c));
                                        }
                                        break;
                                    case Medium:
                                        if ((item.second.c) % 2 == 0) {
                                            gameItems.push_back(GameItem(rect2Use, cubes.getCurrentSphereRef().getMainEye(), gameLevel, item.second.c));
                                        }
                                        break;
                                    case Difficult:
                                        gameItems.push_back(GameItem(rect2Use, cylinders.getCurrentSphereRef().getMainEye(), gameLevel, item.second.c));
                                        break;
                                    case EndGame:
                                        if (item.second.c == 1) { // just a few notes, 1 is a magic note
                                            gameItems.push_back(GameItem(rect2Use, musicNotes.getCurrentSphereRef().getMainEye(), EndGame, item.second.c));
                                            music->keyboard.keyPressed('a');
                                        }
                                        else if (item.second.c == 5) { // just a few notes, 1 is a magic note
                                            gameItems.push_back(GameItem(rect2Use, musicNotes.getCurrentSphereRef().getMainEye(), EndGame, item.second.c));
                                            music->keyboard.keyPressed('g');
                                        }
                                        else if (item.second.c == 7) { // just a few notes, 1 is a magic note
                                            gameItems.push_back(GameItem(rect2Use, musicNotes.getCurrentSphereRef().getMainEye(), EndGame, item.second.c));
                                            music->keyboard.keyPressed('t');
                                        }
                                        else if (item.second.c == 9) { // just a few notes, 1 is a magic note
                                            gameItems.push_back(GameItem(rect2Use, musicNotes.getCurrentSphereRef().getMainEye(), EndGame, item.second.c));
                                            music->keyboard.keyPressed('k');
                                        }
                                        break;
                                    }
                                    break;
                                }
                                else {
                                    if (item.second.c == 1) {
                                        // found, remove it for music
                                        gameItems.remove_if(GameItem::isAkey);
                                        music->keyboard.keyReleased('a');
                                    }
                                    else if (item.second.c == 5) {
                                        // found, remove it for music
                                        gameItems.remove_if(GameItem::isGkey);
                                        music->keyboard.keyReleased('g');
                                    }
                                    else if (item.second.c == 7) {
                                        // found, remove it for music
                                        gameItems.remove_if(GameItem::isTkey);
                                        music->keyboard.keyReleased('t');
                                    }
                                    else if (item.second.c == 9) {
                                        // found, remove it for music
                                        gameItems.remove_if(GameItem::isKkey);
                                        music->keyboard.keyReleased('k');
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (max > shapeMinSize) { // fine tune on site 
            double x = (centroid.x / cameraWidth)*100.0f; // make it a percent
            double y = (centroid.y / cameraHeight)*100.0f; // make it a percent

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