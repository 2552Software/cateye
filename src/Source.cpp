#include "ofApp.h"

void  ImageAnimator::fireWorks() {
    sounds(5);
    ignight(false);

    magicZ = 360 * 10; //bugbug parameter
}
TextTimer::TextTimer(const std::string& textIn, float timeToRenderIn, float delay, float xIn, float yIn) {
    timeDelay = 0;
    done = false;
    text = textIn;
    timeSet = true;
    timeToRender = timeToRenderIn;
    timeDelay = delay;
    timeBegan = (int)ofGetElapsedTimeMillis();
    x = xIn;
    y = yIn;
    holdTextTime = 35.0f;
}
void TextTimer::setup() {


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
            lasting.setDuration(5.0f);
            lasting.setRepeatType(PLAY_ONCE);
            lasting.animateTo(ofColor::orangeRed);
        }
        output = text.substr(0, min(n, max));
        if (output.size() > 0) {
            return true;
        }
    }
    return false;
}

void ImageAnimator::draw(const std::string& s, float x, float y) {
    font.drawStringAsShapes(s, x, y);
}
void ImageAnimator::credits() {
    creditsText.clear();

    std::string s = "Tom And Mark";
    float f = font.stringWidth(s);

    creditsText.push_back(TextTimer(s, 1500.0f, 0.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6)));
    s = "From Eletronic Murals";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 10));

    s = "Would like to Thank Can Can Wonderland ...";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 2*1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 20));

    s = "... for their support of the Arts!";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 3 * 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 30));

    s = "Now go see if they will get you a beer...";
    f = font.stringWidth(s);
    TextTimer t(s, 1500.0f, 8 * 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 40);
    t.holdTextTime = 45.0f;
    creditsText.push_back(t);

    s = "Good bye!";
    f = font.stringWidth(s);
    creditsText.push_back(TextTimer(s, 1500.0f, 11 * 1500.0f, (ofGetScreenWidth() / 2) - (font.stringWidth(s) / 2), (ofGetScreenHeight() / 6) + font.getLineHeight() * 20));


}
void ImageAnimator::drawContours(float cxScreen, float cyScreen) {
    ofSetBackgroundColor(ofColor::black);
    ofSetColor(ofColor::white);

    bool found = false;
    for (auto& credit : creditsText) {
        if (credit.isRunningOrWaitingToRun()) { // if not go to next one
            std::string s;
            if (credit.getString(s)) {
                found = true;
                draw(s, credit.x, credit.y);
            }
        }
        else if (credit.lasting.isAnimating()) {
            found = true;
            ofSetColor(credit.lasting.getCurrentColor());
            draw(credit.text, credit.x, credit.y);
        }
    }
    if (found) {
        return;
    }
    contours.draw(cxScreen, cyScreen);
    int c = count();

    std::stringstream ss;
    ss << c << ":" << winnerCount();
    ofSetWindowTitle(ss.str());

    if (isWinner(c)) { 
       // enable fireworks!!
       fireWorks();
       credits();
    }
    else {
        // else draw boxes but only when its game on
        if (count() >= firstMatchCount() && inFireworks()) {
            for (auto& item : thingsToDo) {
                item.second.draw(175);//bugbug make menu
            }
        }
    }
}

void Map::setup() {
}
void Map::set(int a) {
    action = a;
}

void Map::set(const ofRectangle& r) {
    rectangle = r;
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
void Map::update() {
    color.update(1.0f / ofGetTargetFrameRate());
    game.update(1.0f / ofGetTargetFrameRate());
}

void Map::draw(int alpha) {
    if (isAnimating()) {
        ofPushStyle();
        ofFill();
        ofEnableAlphaBlending();
        //color.applyCurrentColor();
        ofColor c = color.getCurrentColor();
        c.a = alpha;// alpha; keep it light
        ofSetColor(c);
        // convert to screen size
        float xFactor = ofGetScreenWidth() / imgWidth;
        float yFactor = ofGetScreenHeight() / imgHeight;
        ofDrawRectangle(xFactor*rectangle.x, yFactor*rectangle.y, xFactor*rectangle.width, yFactor*rectangle.height);
        ofDisableAlphaBlending();
        ofPopStyle();
    }
}

void ContoursBuilder::setup() {
    vector<ofVideoDevice> devices = video.listDevices();
    for (auto& device : devices) {
        if (device.deviceName.find("facetime") == std::string::npos) {
            video.setDeviceID(device.id);
            break;
        }
    }
    video.setVerbose(true);
    video.setup(imgWidth, imgHeight);
    colorImg.allocate(imgWidth, imgHeight);
    grayImage.allocate(imgWidth, imgHeight);
    grayDiff.allocate(imgWidth, imgHeight);
}

// return true if updated
void ContoursBuilder::update() {
    video.update();
    if (video.isFrameNew()) { // && (ofGetFrameNum() & 1) to slow things down
                              // clear less often
        colorImg.setFromPixels(video.getPixels());
        grayImage = colorImg; // convert our color image to a grayscale image
        if (backgroundImage.bAllocated) {
            grayDiff.absDiff(backgroundImage, grayImage);
        }
        backgroundImage = grayImage; // only track new items -- so eye moves when objects move
        grayDiff.threshold(50); // turn any pixels above 30 white, and below 100 black
        if (!contourFinder.findContours(grayDiff, 5, (imgWidth*imgHeight), 128, false, true)) {
            contourFinder.blobs.clear(); // removes echo but does it make things draw too fast?
        }
        grayImage.blurGaussian(3);
        grayImage.threshold(50);
        if (!contourDrawer.findContours(grayImage, 5, (imgWidth*imgHeight), 128, true)) {
            contourDrawer.blobs.clear();
        }
    }
}

void ContoursBuilder::draw(float cxScreen, float cyScreen) {
    ofPushStyle();
    ofPushMatrix();
    ofNoFill();
    ofSetLineWidth(1);// ofRandom(1, 5));
    for (auto& blob : contourDrawer.blobs) {
        ofPolyline line;
        for (int i = 0; i < blob.nPts; i++) {
            line.addVertex((imgWidth - blob.pts[i].x), blob.pts[i].y);
        }
        line.close();
        line.scale(cxScreen / imgWidth, cyScreen / imgHeight);
        line.draw();
    }
    ofSetLineWidth(5);// ofRandom(1, 5));
    if (contourFinder.blobs.size() > 0) {
        for (auto& blob : contourFinder.blobs) {
            ofPolyline line;
            for (int i = 0; i < blob.nPts; i++) {
                line.addVertex((imgWidth - blob.pts[i].x), blob.pts[i].y);
            }
            line.close();
            line.scale(cxScreen / imgWidth, cyScreen / imgHeight);
            line.draw();
            //ofDrawRectangle(blob.boundingRect.x, blob.boundingRect.y, blob.boundingRect.width, blob.boundingRect.height);
        }
    }
    ofPopMatrix();
    ofPopStyle();
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
void ImageAnimator::circle() {
    ofxAnimatableOfPoint point;
    point.setPosition(currentLocation);
    point.setDuration(1.20f);
    point.animateTo(ofVec3f(1000, 1000, 10));
    path.addTransition(point);
    point.animateTo(ofVec3f(1000, 2000, 200));
    path.addTransition(point);
    point.animateTo(ofVec3f(2000, 2000, -200));
    path.addTransition(point);
    point.animateTo(ofVec3f(1000, 1000, 3000));
    path.addTransition(point);
    point.animateTo(ofVec3f(00, 00));
    path.addTransition(point);
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

void ImageAnimator::buildTable() {
    if (squareCount) {
        thingsToDo.clear();
        // all based on camera size and just grid out the screen 10x10 or what ever size we want
        float w = imgWidth / squareCount; // menu bugbug
        float h = imgHeight / squareCount;
        for (float x = w; x < imgWidth-w; x += w) {
            for (float y = h; y < imgHeight-h; y += h) {
                // roate the x  to reflect viewer vs camera
                thingsToDo.insert(std::make_pair(std::make_pair(x, y), Map(ofRectangle(x, y, w, h)))); // build an default table
            }
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
    shapeMinSize = 100.0f;
    squareCount = 10;
    level = 0;
    magicZ = 0.0f;
}
void ImageAnimator::setup() {
    buildX();
    buildY();
    reset();

    animatorIndex.reset(0.0f);
    animatorIndex.setDuration(1.0f);
    animatorIndex.setRepeatType(LOOP_BACK_AND_FORTH);
    animatorIndex.setCurve(LINEAR);

    path.setup();
    rotator.setup();

    contours.setup();

    string path = DATAPATH;
    ofDirectory allEyes(path);
    allEyes.allowExt("png");
    allEyes.allowExt("jpg");
    size_t i = 0;
    if (allEyes.listDir() > 0) {
        for (; i < allEyes.size(); i++) {
            add(allEyes.getPath(i), allEyes.getName(i));
        }
    }
    else {
        ofLogFatalError() << "eyes missing";
        ofExit(100);
    }
    ofDirectory allSounds(path);
    allSounds.allowExt("wav");
    allSounds.allowExt("mp3");
    allSounds.listDir();
    for (i = 0; i < allSounds.size(); i++) {
        ofSoundPlayer sound;
        sound.load(allSounds.getPath(i));
        mySounds.push_back(sound);
    }

    font.load("alger.ttf", 100, true, true, true);
    font.setLineHeight(18.0f);
    font.setLetterSpacing(1.037);

    startPlaying();

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
void ImageAnimator::windowResized(int w, int h) {
    for (SuperSphere&eye : eyes) {
        eye.setRadius(std::min(w, h));
    }
}

void ImageAnimator::update() {

    if (inFireworks()) {
        magicZ -= 15.0f; //bugbug menu
    }
    if (((int)ofGetElapsedTimef() % 30) == 0) {//bugbug put in menu
        randomize(); // mix up right in the middle of things
    }
    for (auto& a : thingsToDo) {
        a.second.update();
    }
    for (auto&a : creditsText) {
        a.update();
    }
   
    for (SuperSphere&eye : eyes) {
        eye.update();
    }
    animatorIndex.update(1.0f / ofGetTargetFrameRate());
    path.update();
    rotator.update();
    contours.update();

    /* special effects
    if (path.hasFinishedAnimating()) {
       circle();
    }
    */

    // track motion
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

        // see if we have a trigger
        for (auto& blob : contours.contourFinder.blobs) {
            if (blob.area > maxForTrigger && blob.boundingRect.x > 1 && blob.boundingRect.y > 1) {  //x,y 1,1 is some sort of strange case
                int c = count();

                if (c >= firstMatchCount()) {
                    ignight();
                }
                float mymax = maxForTrigger;
                if (c > firstMatchCount()+1) {
                    mymax /= 3; // see less
                }
                if (blob.area >= mymax) {
                    // see if we can trigger with this one
                    for (auto& item : thingsToDo) { // get all blocks within region
                        if (item.second.match(blob.boundingRect)) {
                            item.second.trigger();
                            if (mymax <= maxForTrigger){
                                break; // will make it much harder to get a hit
                            }
                        }
                    }
                }
            }
        }
        if (max > shapeMinSize) { // fine tune on site 
            int w = imgWidth; // camera size not screen size
            int h = imgHeight;

            double x = (centroid.x / imgWidth)*100.0f; // make it a percent
            double y = (centroid.y / imgHeight)*100.0f; // make it a percent

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
        if (max > 10) {
            ofLogNotice() << "insert targert" << target;
            currentRotation = target;
        }
        else {
            // no new data so home the eye (?should we add a time?)
            currentRotation.set(0.0L, 0.0L, 0.0L);
        }
        //std::stringstream ss;
        //ss << max;
        //ofSetWindowTitle(ss.str());

        /*
        if (found && 0) {
            ofxAnimatableOfPoint point;
            // get the current point -- smooth things out
            point.setPosition(currentRotation);
            point.setCurve(LINEAR);
            point.setRepeatType(PLAY_ONCE);
            point.setDuration(0.2f);
            point.animateTo(target);
            rotator.insertTransition(point, true);
        }
        */

    }
}
void ImageAnimator::draw() {

    getCurrentEyeRef().blinkingEnabled = false; // only blink when eye is not doing interesting things bugbug fix blinking
    // move all eyes so when they switch things are current
    if (!path.hasFinishedAnimating()) {
        //currentLocation = path.getPoint();
       // getCurrentEyeRef().blinkingEnabled = false;
// do location later, its just for special effectgs               getCurrentEyeRef().setPosition(currentLocation);
    }
    return;
    // roate current eye as needed
    if (inFireworks()) {
        currentRotation.set(0.0f, 0.0f, magicZ);
        ofScale(1.2f, 1.2f, 1.2f);
        getCurrentEyeRef().blinkingEnabled = false;
    }
    rotate(currentRotation);
    getCurrentEyeRef().draw();
    ofScale(1.0f, 1.0f, 1.0f);

}
void ImageAnimator::startPlaying() {
    animatorIndex.animateTo(eyes.size() - 1);
    sounds();
}
SuperSphere&ImageAnimator::getCurrentEyeRef() {
    return eyes[(int)animatorIndex.getCurrentValue()];
}
