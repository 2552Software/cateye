#include "ofApp.h"
#include "sound.h"

objectTexture&Textures::getCurrentRef() { 
    float f = selector.getCurrentValue();
    //ofLogNotice("eye val") << f << " " << (int)f;
    if (f < skins.size()) {
        return skins[f];
    }
    return skins[0];
}

// let folks know we are in a game
void Game::setTitle() {
    if (inGame()) {
        std::stringstream ss;
        if (winnerThreshold() != (size_t)-1) {
            ss << "Game On! level:" << current.getLevel() << " Find " << winnerHitCount() << ":" << winnerThreshold();
        }
        else {
            ss << "Enjoy the music!";
        }
        ss <<  " " << setprecision(2) << fixed << current.timeLeft();
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

void GameLevel::advance() {

    if (timeLeft() < 0.0f) { // start game every 60 seconds for example
     //bugug for dev keep going forward current = current->getPrevious();
        switch (level) {
        case NoGame:
            setup(Basic);
            break;
        case Basic:
            setup(Medium);
            break;
        case Medium:
            setup(Difficult);
            break;
        case Difficult:
            setup(NoGame);
            break;
        }
    }
}

void  Game::fireWorks() {
   //bugbug sounds(5);
   rotatingEye.getUpAnimator().animateFromTo(-300, 300);//bugbug will need to adjsut for pi
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
    gameEyes.clear();
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
    return gameEyes.size();
}

void Game::rotatingEyesDone(ofxAnimatableFloat::AnimationEvent & event) {
    // no move main eye back into focus
    currentRotation.set(0.0f, 0.0f); // look forward, move ahead its not too late
    mainEye.getUpAnimator().animateFromTo(-rotatingEye.getRadius(), 0.0f);
    clear(); // reset and start again
}
void Game::windowResized(int wIn, int hIn) {
    w = wIn;
    h = hIn;
    ofLogNotice("ofApp::setup") << "Window size " << w << " by " << h;

    // convert to screen size
    xFactor = w / cameraWidth;
    yFactor = h / cameraHeight;
    
    ofRectangle rect(0.0f, 0.0f, w, h);
    mainEye.setRectangle(rect);
    mainEye.setup();
    rotatingEye.setRectangle(rect);
    rotatingEye.setup();

    clear(); // reset game to assure all sizes are correct
   
}

void Game::startPlaying() {
   //bugbug sounds();
}
size_t Game::winnerThreshold() { 
    switch (current.getLevel()) {
    case GameLevel::NoGame:
        return 0;
    case GameLevel::Basic://bugbug drop enum just use numbers 
        return aimationMaps[0].size();
    case GameLevel::Medium:
        return aimationMaps[1].size();
    case GameLevel::Difficult:
        return aimationMaps[2].size();
    }
    return 0;
}

void Game::removeGameItem(int id) {
    gameEyes.erase(std::remove_if(gameEyes.begin(),
        gameEyes.end(),
        [id](std::shared_ptr<EyeGameItem>item) {return item->getID() == id; }),
        gameEyes.end());
}

bool Game::addGameItem(LocationToActionMap* map) {
    if (map) {
        float cx = w - (map->width)*xFactor;
        ofRectangle rect2Use((cx - map->x*xFactor), map->y*yFactor, map->width*xFactor, map->height*yFactor);
        if (!find(rect2Use)) {
            switch (current.getLevel()) {
            case GameLevel::Basic:
                gameEyes.push_back(std::make_shared <EyeGameItem>(rect2Use, spheresSkins.getCurrentRef(), map->c, 60.0f, &mainEye));
                break;
            case GameLevel::Medium:
                gameEyes.push_back(std::make_shared <EyeGameItem>(rect2Use, spheresSkins.getCurrentRef(), map->c, 60.0f, &mainEye));
                break;
            case GameLevel::Difficult:
                gameEyes.push_back(std::make_shared <EyeGameItem>(rect2Use, spheresSkins.getCurrentRef(), map->c, 60.0f, &mainEye));
                break;
            }
        }
    }
    return true;
}
bool Game::find(const ofRectangle& rect) {
    for (auto item : gameEyes) {
        if (*item == rect) {
            return true;
        }
    }
    return false;
}

void Game::getCountours() {
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
                        for (auto& a : aimationMaps) {
                            for (auto& item : a) { // get all blocks within region
                                if (current.getLevel() == item.second.getLevel()) {
                                    if (item.second.inside(blob.boundingRect)) { //
                                        addGameItem(&item.second);
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