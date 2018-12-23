#include "ofApp.h"
#include "sound.h"

objectTexture&Textures::getCurrentRef() { 
    float f = selector.getCurrentValue();
    ofLogNotice("eye val") << f << " " << (int)f;
    return skins[(int)f];
}


// let folks know we are in a game
void Game::setTitle() {
    if (inGame()) {
        std::stringstream ss;
        ss << "Game On! level:" << current->getLevel() <<  " Find " << winnerHitCount() << ":" << winnerThreshold();
        ss <<  " " << setprecision(2) << fixed << current->timeLeft();
        basicText.print(ss.str(), 0.0f, 0.0f, getRadiusGlobal());
   }
}
void Game::blink() {
    // blink upon request
    ofSetColor(ofColor::black);
    ofPushStyle();
    ofFill();
    ofDrawRectangle(0, 0, w, (h / 2)*blinker.val(), getRadiusGlobal());
    ofDrawRectangle(0, h, w, -(h / 2)*blinker.val(), getRadiusGlobal());
    ofPopStyle();
}

void Textures::add(const std::string &name, const std::string &root) {
    skins.push_back(objectTexture(root));
}
std::shared_ptr<GameItem> GameItem::getNext() {
    if (level == NoGame) {
        std::shared_ptr<GameItem> sp{ std::make_shared <SphereGameItem>() };
        sp->resetLevelTime();
        return sp;
    }
    if (level == Basic) {
        std::shared_ptr<GameItem> sp{ std::make_shared <CubeGameItem>() };
        sp->resetLevelTime();
        return sp;
    }
    if (level == Medium) {
        std::shared_ptr<GameItem> sp{ std::make_shared <CylinderGameItem>() };
        sp->resetLevelTime();
        return sp;
    }
    if (level == Difficult) {
        std::shared_ptr<GameItem> sp{ std::make_shared <MusicItem>() };
        sp->resetLevelTime();
        return sp;
    }
    if (level == EndGame) {
        std::shared_ptr<GameItem> sp{ std::make_shared <GameItem>() };
        sp->resetLevelTime();
        return sp;
    }
    return nullptr; // will blow up the app
};

void  Game::fireWorks() {
   //bugbug sounds(5);
   rotatingEye.getAnimator().animateFromTo(-300, 300);//bugbug will need to adjsut for pi
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
    if (gameItems.size() > 0) {
        gameItems.clear();
    }
    sendFireworks = false;
}


void Game::setTriggerCount(float count) {
    if (count > 0) {
        maxForTrigger = count;
    }
}

void Game::setSquareCount(int count) {
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
    mainEye.getAnimator().animateFromTo(-rotatingEye.getRadius(), 0.0f);
    clear(); // reset and start again
}
void Game::windowResized(int wIn, int hIn) {
    w = wIn;
    h = hIn;
    ofLogNotice("ofApp::setup") << "Window size " << w << " by " << h;

    // convert to screen size
    xFactor = w / cameraWidth;
    yFactor = h / cameraHeight;

    mainEye.setup(LOOP_BACK_AND_FORTH, 1.0f, 0.0f, 0.0f, w, h);
    rotatingEye.setup(LOOP_BACK_AND_FORTH, 1.0f, 0.0f, 0.0f, w, h);

    clear(); // reset game to assure all sizes are correct

    
}

void Game::startPlaying() {
   //bugbug sounds();
}
size_t Game::winnerThreshold() { 
    switch (current->getLevel()) {
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

void Game::pushSphere(const ofRectangle&rect, int id) {
    std::shared_ptr<GameItem> sp{ std::make_shared <SphereGameItem>(rect, spheresSkins.getCurrentRef(), &mainEye,id) };
    gameItems.push_back(sp);
}
void Game::pushCube(const ofRectangle&rect, int id) {
    std::shared_ptr<GameItem> sp{ std::make_shared <CubeGameItem>(rect, cubesSkins.getCurrentRef(), &mainEye,id) };
    gameItems.push_back(sp);
}
void Game::pushCylinder(const ofRectangle&rect, int id) {
    std::shared_ptr<GameItem> sp{ std::make_shared <CylinderGameItem>(rect, cylindersSkins.getCurrentRef(), &mainEye,id) };
    gameItems.push_back(sp);
}
void Game::pushMusic(const ofRectangle&rect, int id) {
    std::shared_ptr<GameItem> sp{ std::make_shared <MusicItem>(rect, musicNotesSkins.getCurrentRef(), &mainEye,id) };
    gameItems.push_back(sp);
}

bool Game::compute(LocationToInfoMap rect, Music*music) {
    float cx = w - (rect.width)*xFactor;
    ofRectangle rect2Use((cx - rect.x*xFactor), rect.y*yFactor, rect.width*xFactor, rect.height*yFactor);
    if (!find(rect2Use)) {
        switch (current->getLevel()) {
        case Basic: 
            pushSphere(rect2Use, rect.c);
            break;
        case Medium:
            pushCube(rect2Use, rect.c);
            break;
        case Difficult: // cylinder game
            pushCylinder(rect2Use, rect.c);
            break;
        case EndGame: //bugbug make 8 octaves
            if (rect.c == 1) { // just a few notes, 1 is a magic note
                music->keyboard.keyPressed('a');
            }
            else if (rect.c == 5) { // just a few notes, 1 is a magic note
                music->keyboard.keyPressed('g');
            }
            else if (rect.c == 7) { // just a few notes, 1 is a magic note
                music->keyboard.keyPressed('t');
            }
            else if (rect.c == 9) { // just a few notes, 1 is a magic note
                music->keyboard.keyPressed('k');
            }
            pushMusic(rect2Use, rect.c);
            break;
        }
    }
    else {
        if (rect.c == 1) {
            // found, remove it for music
            gameItems.remove_if(MusicItem::isAkey);
            music->keyboard.keyReleased('a');
        }
        else if (rect.c == 5) {
            // found, remove it for music
            gameItems.remove_if(MusicItem::isGkey);
            music->keyboard.keyReleased('g');
        }
        else if (rect.c == 7) {
            // found, remove it for music
            gameItems.remove_if(MusicItem::isTkey);
            music->keyboard.keyReleased('t');
        }
        else if (rect.c == 9) {
            // found, remove it for music
            gameItems.remove_if(MusicItem::isKkey);
            music->keyboard.keyReleased('k');
        }
        return true;
    }
    return false;
}
bool Game::find(const ofRectangle& rect) {
    for (auto item : gameItems) {
        if (*item == rect) {
            return true;
        }
    }
    return false;
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
                                if (compute(item.second, music)) {
                                    break;
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