#include "ofApp.h"

// sound https://www.liutaiomottola.com/formulae/freqtab.htm
void ImageAnimator::buildTable() {
    if (squareCount) {
        cameraMapping.clear();
        float w = cameraWidth / squareCount; // menu bugbug
        float h = cameraHeight / squareCount;
        const int max = 14;

        float freqs[max] = {
            82.407, 87.307f, 92.499f, 97.999f, 103.826f, 110.0f, 116.541f, 123.471f, 130.813f, 138.591f, 146.8325f,
            155.563f, 164.814, 334.648f
        };
        /*
            F	3	174.614	1.949m
            F#  / Gb	3	184.997	1.839m
            G	3	195.998	1.736m	Lowest note of violin
            G#  / Ab	3	207.652	1.639m
            A	3	220	1.547m
            A#  / Bb	3	233.082	1.46m
            B	3	246.942	1.378m

            C	4	261.626	1.301m	Middle C
            C#  / Db	4	277.183	1.228m
            D	4	293.665	1.159m
            D#  / Eb	4	311.127	1.094m
            E	4	329.628	1.032m
            F	4	349.228	0.974m
            F#  / Gb	4	369.994	0.92m
            G	4	391.995	0.868m
            G#  / Ab	4	415.305	0.819m
            A	4	440	0.773m	Tuning reference note
            A#  / Bb	4	466.164	0.73m
            B	4	493.883	0.689m

            C	5	523.251	0.65m
            C#  / Db	5	554.365	0.614m
            D	5	587.33	0.579m
            D#  / Eb	5	622.254	0.547m
            E	5	659.255	0.516m
            F	5	698.456	0.487m
            F#  / Gb	5	739.989	0.46m
            G	5	783.991	0.434m
            G#  / Ab	5	830.609	0.41m
            A	5	880	0.387m
            A#  / Bb	5	932.328	0.365m
            B	5	987.767	0.345m

            C	6	1046.502	0.325m
            C#  / Db	6	1108.731	0.307m
            D	6	1174.659	0.29m
            D#  / Eb	6	1244.508	0.273m
            E	6	1318.51	0.258m
            F	6	1396.913	0.244m
            F#  / Gb	6	1479.978	0.23m
            G	6	1567.982	0.217m
            G#  / Ab	6	1661.219	0.205m
            A	6	1760	0.193m
            A#  / Bb	6	1864.655	0.182m
            B	6	1975.533	0.172m

            C	7	2093.005	0.163m
            C#  / Db	7	2217.461	0.153m
            D	7	2349.318	0.145m
            D#  / Eb	7	2489.016	0.137m
            E	7	2637.021	0.129m
            F	7	2793.826	0.122m
            F#  / Gb	7	2959.955	0.115m
            G	7	3135.964	0.109m
            G#  / Ab	7	3322.438	0.102m
            A	7	3520	0.097m
            A#  / Bb	7	3729.31	0.091m
            B	7	3951.066	0.086m

            C	8	4186.009	0.081m
            C#  / Db	8	4434.922	0.077m
            D	8	4698.636	0.072m
            D#  / Eb	8	4978.032	0.068m
            E	8	5274.042	0.065m
            F	8	5587.652	0.061m
            F#  / Gb	8	5919.91	0.057m
            G	8	6271.928	0.054m
            G#  / Ab	8	6644.876	0.051m
            A	8	7040	0.048m
            A#  / Bb	8	7458.62	0.046m
            B	8	7902.132	0.043m

            C	9	8372.018	0.041m
            C#  / Db	9	8869.844	0.038m
            D	9	9397.272	0.036m
            D#  / Eb	9	9956.064	0.034m
            E	9	10548.084	0.032m
            F	9	11175.304	0.03m
            F#  / Gb	9	11839.82	0.029m
            G	9	12543.856	0.027m
            G#  / Ab	9	13289.752	0.026m
            A	9	14080	0.024m
            A#  / Bb	9	14917.24	0.023m
            B	9	15804.264
                    */

        // all based on camera size and just grid out the screen 10x10 or what ever size we want
        int i = 0;
        for (float x = w; x < cameraWidth-w; x += w) { // keep off the edges -- camera cannot always pick those up
            for (float y = h; y < cameraHeight-h; y += h) {
                LocationToInfoMap map;
                map.x = x;
                map.y = y;
                map.width = w;
                map.height = h;
                map.frequency = freqs[i];
                if (++i >= max) {
                    i = 0;
                }
                cameraMapping.insert(std::make_pair(std::make_pair(x, y), map)); // build a default table
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

