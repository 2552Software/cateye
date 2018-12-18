#include "ofApp.h"

// sound https://www.liutaiomottola.com/formulae/freqtab.htm
void ImageAnimator::buildTable() {
    if (squareCount) {
        screenToAnimationMap.clear();
        float w = cameraWidth / squareCount; // menu bugbug
        float h = cameraHeight / squareCount;

        float freqs[] = {
            82.407, 87.307f, 92.499f, 97.999f, 103.826f, 110.0f, 116.541f, 123.471f, 130.813f, 138.591f, 146.8325f,155.563f, 164.814, 174.614, 184.997, 
            195.998, 207.652, 220.0, 233.082,246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.0,466.164,
            493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 880.0, 932.328,  987.767, 1046.502, 1108.731, 1174.659, 
            1244.50, 1318.51,1396.913,1479.978,1567.982,1661.219,1760.0, 1864.655, 1975.533,2093.005,2217.461,2349.318,2489.016,2637.021,2793.826,
            2793.826,2959.955, 3135.964,3322.438,3520.0, 3729.31,3951.066,4186.009,4434.922,4698.636,4978.032,5274.042,5587.652, 5919.91,6271.92,6644.876,7040.0, 
            7458.62,7902.132,8372.018,8869.844,9397.272,9956.064,10548.084,11175.304, 11839.82, 12543.856, 12543.856, 13289.752, 14917.24, 15804.264
        };
        const int max = sizeof(freqs) / sizeof(float);
        // all based on camera size and just grid out the screen 10x10 or what ever size we want
        int i = max;
        LocationToInfoMap map;
        map.width = w;
        map.height = h;
        float vol = 0.0f;
        for (float x = w; x < cameraWidth-w; x += w) { // keep off the edges -- camera cannot always pick those up
            for (float y = h; y < cameraHeight-h; y += h) {
                map.x = x;
                map.y = y;
                if (--i < 0) {
                    i = max;
                    vol += 0.2f;
                    map.music.volume = vol;
                }
                map.music.frequency = freqs[i];
                screenToAnimationMap.insert(std::make_pair(std::make_pair(x, y), map)); // build a default table
            }
        }
    }
}

void ImageAnimator::buildX() {
    float percent = 0.0f;// location as a percent of screen size
    float r = 30.0f; // rotation
    float incPercent = 5.0f;
    float incRotaion = ((r * 2) / (100.0f / incPercent - 1));
    for (int i = 1; percent < 100.0f; ++i, percent += incPercent, r -= incRotaion) {
        mapCameraInX.insert(std::make_pair(std::make_pair(percent, percent + incPercent), r));
    }
}

void ImageAnimator::buildY() {
    float percent = 0.0f;// location as a percent of screen size
    float r = -20.0f; // rotation
    float incPercent = 5.0f;
    float incRotaion = ((r * 2) / (100.0f / incPercent - 1));
    for (int i = 1; percent < 100.0f; ++i, percent += incPercent, r -= incRotaion) {
        mapCameraInY.insert(std::make_pair(std::make_pair(percent, percent + incPercent), r));
    }
}

TextTimer::TextTimer(const std::string& textIn, float timeToRenderIn, float delay, float lineIn) {
    line = lineIn;
    timeDelay = 0;
    done = false;
    text = textIn;
    timeSet = true;
    timeToRender = timeToRenderIn;
    timeDelay = delay;
    timeBegan = (int)ofGetElapsedTimeMillis();
    holdTextTime = 35.0f;
}

void ImageAnimator::circle() {
    ofxAnimatableOfPoint point;
    point.setPosition(currentLocation);
    point.setDuration(1.20f);
    point.animateTo(ofVec3f(1000, 1000, 10));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(1000, 2000, 200));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(2000, 2000, -200));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(1000, 1000, 3000));
    imagPath.addTransition(point);
    point.animateTo(ofVec3f(00, 00));
    imagPath.addTransition(point);
}

bool TextTimer::getString(std::string& output) {
    output.clear();
    int elapsedSeconds = ((int)ofGetElapsedTimeMillis() - timeBegan); //  20 seconds passed for example timeDelay
    if (timeDelay) {
        if (elapsedSeconds < timeDelay) {
            return false;
        }
        else {
            timeBegan = (int)ofGetElapsedTimeMillis(); // here we go
            timeDelay = 0.0f; // needs to be rest if used again
            return false; // get it next time
        }
    }
    if (!elapsedSeconds) {
        return false;
    }
    if (text.size() == 0 || timeToRender <= 0) {
        return false;
    }
    if (!done) {
        float factor = ((float)(elapsedSeconds) / (float)timeToRender);  // ratio of seconds that passed to our full range of time, say 20% or 0.2

        int n = (int)(factor * text.length());
        int max = min(n, (int)text.length());

        if (!n) {
            max = 1;
        }
        if (n >= (int)text.length()) {
            done = true;
            lasting.setColor(ofColor::white);
            lasting.setDuration(1.0f);
            lasting.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
            lasting.setCurve(EASE_IN_EASE_OUT);
            lasting.animateTo(ofColor::orangeRed);
        }
        output = text.substr(0, min(n, max));
        if (output.size() > 0) {
            return true;
        }
    }
    return false;
}

std::string ImageAnimator::sillyString() {
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
void ImageAnimator::credits(bool signon) {
    /** bugbug removed */
    displayText.push_back(TextTimer("fast for testing", 1000.0f, 0.0f, 0.0f));
    return;

    displayText.clear();

    if (signon || (int)ofRandom(0, 10) > 2) {
        displayText.push_back(TextTimer("Tom And Mark", 1000.0f, 0.0f, 0.0f));
        displayText.push_back(TextTimer("From Electronic Murals", 1000.0f, 1000.0f, 1.0f));
        displayText.push_back(TextTimer("Thank Can Can Wonderland ...", 1000.0f, 2 * 1000.0f, 2.0f));
        displayText.push_back(TextTimer("... For their support of the Arts!", 1000.0f, 3 * 1000.0f, 3.0f));
    }
    else {
        displayText.push_back(TextTimer(sillyString(), 1000.0f, 0.0f, 0.0f));
    }
}

void Eye::start() {
    material.begin();
    bind();
}
void Eye::stop() {
    unbind();
    material.end();
}

