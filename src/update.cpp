#include "ofApp.h"

void Eyes::update() {
    for (SuperSphere&eye : eyes) {
        eye.update();
    }
    getAnimator().update(1.0f / ofGetTargetFrameRate());
    selector.update(1.0f / ofGetTargetFrameRate());
}

void GameItem::update() {
    color.update(1.0f / ofGetTargetFrameRate());
    box.rollDeg(15.0f);
}

void ImageAnimator::update() {

    for (auto& a : gameItems) {
        a.update();
    }

    for (auto&a : creditsText) {
        a.update();
    }

    mainEyes.update();
    rotatingEyes.update();

    imagPath.update();
    rotator.update();
    contours.update();

    if (!isAnimating()) {
        int c = winnerHitCount();
        std::stringstream ss;
        ss << c << ":" << winnerThreshold();
        ofSetWindowTitle(ss.str());
        if (isWinner(c)) {
            //credits will call fireworks when done
            sendFireworks = true;
            credits();
        }
        else {
            if (((int)ofGetElapsedTimef() % 30) == 0) {//bugbug put in menu
                randomize(); // mix up right in the middle of things
            }
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
                        int c = winnerHitCount();

                        if (c >= firstMatchCount()) { //bugbug what to do here?
                        }

                        float mymax = maxForTrigger;
                        if (c > firstMatchCount() + 1) {
                            mymax /= 3; // see less once game stgarts
                        }
                        if (blob.area >= mymax) {
                            // see if we can trigger with this one
                            for (auto& item : cameraMapping) { // get all blocks within region
                                if (item.second.match(blob.boundingRect)) {
                                    gameItems.push_back(ofRectangle(blob.boundingRect.x*xFactor, blob.boundingRect.y*yFactor, blob.boundingRect.width*xFactor, blob.boundingRect.height*yFactor));
                                    if (mymax <= maxForTrigger) {
                                        break; // will make it much harder to get a hit
                                    }
                                }
                            }
                        }
                    }
                }
                if (max > shapeMinSize) { // fine tune on site 
                    int w = cameraWidth; // camera size not screen size
                    int h = cameraHeight;

                    double x = (centroid.x / cameraWidth)*100.0f; // make it a percent
                    double y = (centroid.y / cameraHeight)*100.0f; // make it a percent

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
                    //   ofLogNotice() << "insert targert" << target;
                    currentRotation = target;
                }
                else {
                    // no new data so home the eye (?should we add a time?)
                    currentRotation.set(0.0L, 0.0L, 0.0L);
                }
            }
        }
    }
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
        grayDiff.threshold(50); // turn any pixels above 50 white, and below 100 black bugbug menu can tune game here too
        if (!contourFinder.findContours(grayDiff, 5, (cameraWidth*cameraHeight), 128, false, true)) {
            contourFinder.blobs.clear(); // removes echo but does it make things draw too fast?
        }
        grayImage.blurGaussian(3);
        grayImage.threshold(50);
        if (!contourDrawer.findContours(grayImage, 5, (cameraWidth*cameraHeight), 128, true)) {
            contourDrawer.blobs.clear();
        }
    }
}

