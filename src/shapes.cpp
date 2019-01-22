#include "ofApp.h"

void Game::buildTable() {
    if (squareCount) {
        aimationMaps.clear();
        aimationMaps.resize(3);
        float w = cameraWidth / 4; // menu bugbug
        float h = cameraHeight / 4;
        LocationToActionMap map;
        map.set(GameLevel::Basic);
        map.width = w;
        map.height = h;
        for (float x = 0; x < cameraWidth; x += w) { // keep off the edges -- camera cannot always pick those up
            map.x = x;
            for (float y = 0; y < cameraHeight; y += h) {
                map.y = y;
                aimationMaps[0].insert(std::make_pair(std::make_pair(x, y), map)); // build a default table
                map.c++;
            }
        }
        map.set(GameLevel::Medium);
        w = cameraWidth / 5; // menu bugbug
        h = cameraHeight / 5;
        map.width = w;
        map.height = h;
        for (float x = 0; x < cameraWidth; x += w) { // keep off the edges -- camera cannot always pick those up
            map.x = x;
            for (float y = 0; y < cameraHeight; y += h) {
                map.y = y;
                aimationMaps[1].insert(std::make_pair(std::make_pair(x, y), map)); // build a default table
                map.c++;
            }
        }
        map.set(GameLevel::Difficult);
        w = cameraWidth / 6; //a little more difficult
        h = cameraHeight / 6;
        map.width = w;
        map.height = h;
        for (float x = 0; x < cameraWidth; x += w) { // keep off the edges -- camera cannot always pick those up
            map.x = x;
            for (float y = 0; y < cameraHeight; y += h) {
                map.y = y;
                aimationMaps[2].insert(std::make_pair(std::make_pair(x, y), map)); // build a default table
                map.c++;
            }
        }
    }
}

void Game::buildX() {
    float percent = 0.0f;// location as a percent of screen size
    float r = 30.0f; // rotation
    float incPercent = 5.0f;
    float incRotaion = ((r * 2) / (100.0f / incPercent - 1));
    for (int i = 1; percent < 100.0f; ++i, percent += incPercent, r -= incRotaion) {
        mapCameraInX.insert(std::make_pair(std::make_pair(percent, percent + incPercent), r));
    }
}

void Game::buildY() {
    float percent = 0.0f;// location as a percent of screen size
    float r = -20.0f; // rotation
    float incPercent = 5.0f;
    float incRotaion = ((r * 2) / (100.0f / incPercent - 1));
    for (int i = 1; percent < 100.0f; ++i, percent += incPercent, r -= incRotaion) {
        mapCameraInY.insert(std::make_pair(std::make_pair(percent, percent + incPercent), -r));
    }
}

TextTimer::TextTimer(const std::string& text, unsigned int timeToRenderIn, unsigned int delay, int lineIn) {
    line = lineIn;
    rawText = text;
    timeDelay = delay;
    elapsed = 0;
    lingerTime = timeToRenderIn;// text remains  after animation
}

std::string& TextTimer::getPartialString() {
    return finalText;
}

std::string TextEngine::sillyString() {
    //bugbug add more stuff
    std::string s;
    switch ((int)ofRandom(0, 18)) {
    case 0:
        s = "Time 4 Free beer???";
        break;
    case 1:
        s = "cookie time?";
        break;
    case 2:
        s = "Is it bar time? prrfect";
        break;
    case 3:
        s = "Want to play a game?";
        break;
    case 4:
        s = "Hal open the pod bay door";
        break;
    case 5:
        s = "pinball!!";
        break;
    case 6:
        s = "Never trust a dog!!";
        break;
    case 7:
        s = "<your joke here>";
        break;
    case 8:
        s = "Can Can Wonderland Rules!";
        break;
    case 9:
        s = "0101010101010110110101001101001001!";
        break;
    case 10:
        s = "Hello Can Can Wonderland!";
        break;
    case 11:
        s = "What's your OS?";
        break;
    case 12:
        s = "Cats Rule!";
        break;
    case 13:
        s = "MEOW!";
        break;
    case 15:
        s = "Pinball or mini golf next?";
        break;
    case 16:
        s = "Beer can reduce a hangover";
        break;
    case 17:
        s = "Howdy howdy!";
        break;
    default:
        s = "Time for a refill!";
        break;
    }
    return s;
}
void Game::credits(bool signon) {
    /** bugbug removed */
   // fancyText.addFullScreenText(TextTimer("fast for testing", 1000.0f, 0.0f, 0.0f));
    //return;
    fancyText.clear();
    if (signon) {
        playSound("fanfare");
        fancyText.addFullScreenText(TextTimer("Tom And Mark", (int)ofGetTargetFrameRate()*2, 0, 2));
        fancyText.addFullScreenText(TextTimer("From Electronic Murals", (int)ofGetTargetFrameRate() * 3, 0, 3));
        fancyText.addFullScreenText(TextTimer("Thank Can Can Wonderland ...", (int)ofGetTargetFrameRate()*4,0, 4));
        fancyText.addFullScreenText(TextTimer("... For their support of the Arts!", (int)ofGetTargetFrameRate()*5, 0, 5));
    }
    else {
        fancyText.addFullScreenText(TextTimer(fancyText.sillyString(), (int)ofGetTargetFrameRate()*2, 0, 4));
    }
}

void objectTexture::start() {
    if (!isAllocated()) {
        ofLogError("Eye::start") << "not loaded ";
        return;
    }
    material.begin();
    bind();
}
void objectTexture::stop() {
    unbind();
    material.end();
}

