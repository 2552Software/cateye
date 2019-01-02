#include "ofApp.h"

void Game::buildTable() {
    if (squareCount) {
        aimationMap.clear();
        float w = cameraWidth / squareCount; // menu bugbug
        float h = cameraHeight / squareCount;
        LocationToActionMap map;
        map.width = w;
        map.height = h;
        for (float x = w; x < cameraWidth-w; x += w) { // keep off the edges -- camera cannot always pick those up
            map.x = x;
            for (float y = h; y < cameraHeight-h; y += h) {
                map.y = y;
                aimationMap.insert(std::make_pair(std::make_pair(x, y), map)); // build a default table
                map.c++;
            }
        }
        LocationToMusicMap musicmap;
        musicmap.width = cameraWidth / 8; // always 8x8
        musicmap.height = cameraHeight / 8;
        float music[][2] = { {0,0}, {1,2}, {2,4}, {3,6},{4,8} }; //bugbug generate pitch and amp
        for (float x = musicmap.width; x < cameraWidth - musicmap.width; x += musicmap.width) { // keep off the edges -- camera cannot always pick those up
            musicmap.x = x;
            for (float y = musicmap.height; y < cameraHeight - musicmap.height; y += musicmap.height) {
                musicmap.y = y;
                musicmap.pitch = ofMap(x, 0, cameraHeight, 36.0f, 172.0f);
                musicmap.trig = ofMap(y, 0, cameraWidth, 1.0f, 0.000001f);
                musicmap.amp = ofMap(y, 0, cameraWidth, 1.0f, 0.000001f);
                musicMap.insert(std::make_pair(std::make_pair(x, y), musicmap)); // build a default table
                musicmap.c++;
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
    doneDrawing = false;
    rawText = text;
    timeToRender = timeToRenderIn;
    timeDelay = delay;
    timeBegan = (int)ofGetSystemTimeMillis();
    lingerTime = timeDelay+timeToRender+5000.0;// text remains for 1 second after animation
}

std::string& TextTimer::getPartialString() {
    return partialText;
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
   // fancyText.addFullScreenText(TextTimer("fast for testing", 1000.0f, 0.0f, 0.0f));
    //return;

    if (signon || (int)ofRandom(0, 10) > 2) {
        fancyText.addFullScreenText(TextTimer("Tom And Mark", 1000.0f, 0.0f, 0.0f));
        fancyText.addFullScreenText(TextTimer("From Electronic Murals", 1000.0f, 1000.0f,  1.0f));
        fancyText.addFullScreenText(TextTimer("Thank Can Can Wonderland ...", 1000.0f, 2 * 1000.0f, 2.0f));
        fancyText.addFullScreenText(TextTimer("... For their support of the Arts!", 1000.0f, 3 * 1000.0f, 3.0f));
    }
    else {
        fancyText.addFullScreenText(TextTimer(fancyText.sillyString(), 1000.0f, 0.0f,  0.0f));
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

