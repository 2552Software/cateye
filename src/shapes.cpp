#include "ofApp.h"

// sound https://www.liutaiomottola.com/formulae/freqtab.htm
void Game::buildTable() {
    if (squareCount) {
        screenToAnimationMap.clear();
        float w = cameraWidth / squareCount; // menu bugbug
        float h = cameraHeight / squareCount;
        LocationToInfoMap map;
        map.width = w;
        map.height = h;
        for (float x = w; x < cameraWidth-w; x += w) { // keep off the edges -- camera cannot always pick those up
            map.x = x;
            for (float y = h; y < cameraHeight-h; y += h) {
                map.y = y;
                screenToAnimationMap.insert(std::make_pair(std::make_pair(x, y), map)); // build a default table
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

TextTimer::TextTimer(const std::string& text, float timeToRenderIn, float delay, float lineIn) {
    line = lineIn;
    timeDelay = 0;
    done = false;
    rawText = text;
    timeSet = true;
    timeToRender = timeToRenderIn;
    timeDelay = delay;
    timeBegan = (int)ofGetSystemTimeMillis();
}

bool TextTimer::getString(std::string& output) {
    output.clear();
    if (partialText.size() > 0) {
        output = partialText;
        return true;
    }
    return false;
}

std::string TextEngine::sillyString() {
    //bugbug add more stuff
    std::string s;
    switch ((int)ofRandom(0, 18)) {
    case 0:
        s = "Free beer???";
        break;
    case 1:
        s = "cookie time?";
        break;
    case 2:
        s = "Is it bar time?";
        break;
    case 3:
        s = "Want to play a game?";
        break;
    case 4:
        s = "Hal, is that you? Hal? Hal? Way in the back?";
        break;
    case 5:
        s = "pinball!!";
        break;
    case 6:
        s = "I do not have that many random strings";
        break;
    case 7:
        s = "<your joke here>";
        break;
    case 8:
        s = "Can Can Wonderland Rules!";
        break;
    case 9:
        s = "0101010101010101001011010101001101001001!";
        break;
    case 10:
        s = "Does A Squared + B Squared does really = C Squared every time?";
        break;
    case 11:
        s = "What's your OS?";
        break;
    case 12:
        s = "E = MC Squared? what? no way";
        break;
    case 13:
        s = "I’d like to help you out... Which way did you come in?";
        break;
    case 15:
        s = "HippopotomonstrosesquiPedaliophobia is the fear of long words. Scary.";
        break;
    case 16:
        s = "Beer can reduce a hangover";
        break;
    case 17:
        s = "Howdy howdy!";
        break;
    default:
        s = "Take the default action where ever you can";
        break;
    }
    return s;
}
void Game::credits(bool signon) {
    /** bugbug removed */
    text.addFullScreenText(TextTimer("fast for testing", 1000.0f, 0.0f, 0.0f));
    return;

    if (signon || (int)ofRandom(0, 10) > 2) {
        text.addFullScreenText(TextTimer("Tom And Mark", 1000.0f, 0.0f, 0.0f));
        text.addFullScreenText(TextTimer("From Electronic Murals", 1000.0f, 1000.0f,  1.0f));
        text.addFullScreenText(TextTimer("Thank Can Can Wonderland ...", 1000.0f, 2 * 1000.0f, 2.0f));
        text.addFullScreenText(TextTimer("... For their support of the Arts!", 1000.0f, 3 * 1000.0f, 3.0f));
    }
    else {
        text.addFullScreenText(TextTimer(text.sillyString(), 1000.0f, 0.0f,  0.0f));
    }
}

void Eye::start() {
    if (!isAllocated()) {
        ofLogError("Eye::start") << "not loaded ";
        return;
    }
    material.begin();
    bind();
}
void Eye::stop() {
    unbind();
    material.end();
}

