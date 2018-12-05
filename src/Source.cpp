#include "ofApp.h"

void ImageAnimator::drawContours(float cxScreen, float cyScreen) {
    contours.draw(cxScreen, cyScreen);
    int c = count();
    if (match(c)) {
        reset();
       sounds(3);//  sound in menu, same as grids time out etc bugbug
    }
    else {
        // else draw boxes as hints bugbug make boxes smaller
        for (auto& item : thingsToDo) {
            item.second.draw(c);
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
    if (action > 0 && !isAnimating()) {
        game.reset(50.0f);
        game.setCurve(LINEAR);
        game.setRepeatType(LOOP_BACK_AND_FORTH); //bugbug menu -- this mode never stops
        game.setDuration(15.0f);
        game.animateTo(200.0f);
        ofColor c1(0, 255, 255); // bugbug randomize?
        ofColor c2(255, 0, 255);
        color.setAlphaOnly(game); // fade in
        color.animateToAlpha(game);
        color.setColor(c1);
        color.setDuration(3.0f);
        color.setRepeatType(LOOP_BACK_AND_FORTH);
        color.setCurve(LINEAR);
        color.animateTo(c2);
    }
}
void Map::update() {
    color.update(1.0f / ofGetTargetFrameRate());
    game.update(1.0f / ofGetTargetFrameRate());
}

void Map::draw(int count) {
    if (isAnimating()) {
        ofPushStyle();
        ofFill();
        ofEnableAlphaBlending();
        //color.applyCurrentColor();
        ofColor c = color.getCurrentColor();
        c.a = count*2;
        ofSetColor(c);
        // convert to screen size
        float xFactor = ofGetScreenWidth()/ imgWidth;
        float yFactor = ofGetScreenHeight()/ imgHeight;
        ofDrawRectangle(xFactor*rectangle.x, yFactor*rectangle.y, xFactor*rectangle.width, yFactor*rectangle.height);
        ofDisableAlphaBlending();
        ofPopStyle();
    }
    else {
        int i = 0;
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
   // if (fabs(target.x) > 16.0) draw less
    if (target.x || target.y || target.z) {
        ofLogNotice() << "rotate to targert" << target;
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

// call just after things are found and upon startup
void ImageAnimator::randomize() {
    // create hot grids
    for (auto& a : thingsToDo) {
        a.second.set(0); // clear all is 0 menu pick -- all 1s enable all
    }
    
    // make sure we get 3 random points used to unlock the game
    for (int c = 0; c < 3; ) {
        int i = (int)ofRandom(0, thingsToDo.size() - 1);
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

void ImageAnimator::setup() {
    ofSetFrameRate(60.0f);
    buildX();
    buildY();
    // all based on camera size and just grid out the screen 10x10 or what ever size we want
    float w = imgWidth / 20; // menu bugbug
    float h = imgHeight / 20;
    for (float x = 0.0f; x < imgWidth; x += w) {
        for (float y = 0.0f; y < imgHeight; y += h) {
            // roate the x  to reflect viewer vs camera
            thingsToDo.insert(std::make_pair(std::make_pair(x, y), Map(ofRectangle(x, y, w, h)))); // build an default table
        }
    }
    
    randomize();

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
    for (auto& item : thingsToDo) {
        item.second.reset();
    }
    randomize();
}
void ImageAnimator::windowResized(int w, int h) {
    for (SuperSphere&eye : eyes) {
        eye.setRadius(std::min(w, h));
    }
}

void ImageAnimator::update() {
    for (auto& a : thingsToDo) {
        a.second.update();
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
    if (rotator.hasFinishedAnimating()) {
        ofxAnimatableOfPoint point;
        point.setPosition(currentRotation);
        point.animateTo(ofVec3f(ofRandom(90.0f), ofRandom(90.0f)));
        rotator.addTransition(point);
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

        // find max size
        for (auto& blob : contours.contourFinder.blobs) {
            if (blob.area > 5 && blob.boundingRect.x > 1 && blob.boundingRect.y > 1) {  //x,y 1,1 is some sort of strange case
                for (auto& item : thingsToDo) {
                    if (item.second.match(blob.boundingRect)) {
                        item.second.trigger();
                    }
                }
            }
        }
        if (max > 100) { // fine tune on site bugbug put in menu as well as light color range
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
        std::stringstream ss;
        ss << max;
        ofSetWindowTitle(ss.str());

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


    // roate current eye as needed
    //if (!rotator.hasFinishedAnimating()) {
    //    currentRotation = rotator.getPoint();
    //    getCurrentEyeRef().blinkingEnabled = false;
   // }
    rotate(currentRotation);
    getCurrentEyeRef().draw();

}
void ImageAnimator::startPlaying() {
    animatorIndex.animateTo(eyes.size() - 1);
    sounds();
}
SuperSphere&ImageAnimator::getCurrentEyeRef() {
    return eyes[(int)animatorIndex.getCurrentValue()];
}
