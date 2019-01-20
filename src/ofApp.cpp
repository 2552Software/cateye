#include "ofApp.h"
#include "sound.h"
//Music *music = nullptr;
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLogToConsole();

    //ofSetFullscreen(true);
    //ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());

    hideMenu = true;
    ofSetFrameRate(15.0f); 

    ofLogNotice("ofApp::setup") << "of version " << ofGetVersionInfo();

    //ofEnableSeparateSpecularLight();

    //ofEnableLighting();
    ofSetVerticalSync(true);
    ofDisableArbTex();
    //ofSetSmoothLighting(true);
  
    // camera.lookAt(eyeAnimator.sphere);
    light.setup();
    // Works like shit on 4k a does most of OF.
    //gui.setup();
   // gui.setBackgroundColor(ofColor::white);
   // gui.setFillColor(ofColor::blue);

    // setup(const std::string& collectionName = "", const std::string& filename = ofxPanelDefaultFilename, float x = 10, float y = 10);

   // gui.add(squareCount.setup("Squares", 15, 15, 100));
   // gui.add(maxForTrigger.setup("Triggers", 50.0f, 200.0f, 100.0f));
   // gui.add(maxForShape.setup("Shapes", 100.0f, 200.0f, 500.f));

    //bugbug figure out a UI later eyeAnimator.setCount(squareCount);
    //eyeAnimator.setShapeMinSize(maxForShape);
    //eyeAnimator.setTriggerCount(maxForTrigger);
    eyeAnimator.setup();
    music = new Music();
    if (music) {
        music->setup(cameraWidth, cameraHeight); // tie to app
        music->engine.stop();
    }

    //player = new AudioPlayer; //bugbug replace cat calls with this
    //player->load("wargames.wav");
    //player->out("0") >> music->engine.audio_out(0);
    //player->play();
    


    eyeAnimator.credits(true); // setup credits, shown at boot bugbug spin eyes at boot too -- and restore text

   // gui.loadFont(OF_TTF_SANS, 24, true, true);
   // gui.setBorderColor(ofColor::yellow);
   // gui.setTextColor(ofColor::black);
   // gui.setHeaderBackgroundColor(ofColor::orangeRed);
   // gui.setBackgroundColor(ofColor::yellowGreen);
   // gui.setPosition(ofGetWidth() / 2, ofGetHeight() / 2);
   // gui.setShape(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 10, ofGetHeight() / 10 );
   // gui.loadFromFile("settings.xml");

    //squareCount.addListener(this, &ofApp::squareCountChanged);
    //maxForTrigger.addListener(this, &ofApp::triggerCountChanged);
    //maxForShape.addListener(this, &ofApp::shapeSizeChanged);
}

void ofApp::shapeSizeChanged(float &size) {
    // menu is next release bugbugeyeAnimator.setShapeMinSize(size);
}

void ofApp::triggerCountChanged(float &count) {
    // menu is next release bugbugeyeAnimator.setTriggerCount(count);
}

void ofApp::squareCountChanged(int &squareCount) {
    // menu is next release bugbug eyeAnimator.setCount(squareCount); 
}
void ofApp::windowResized(int w, int h) {
    eyeAnimator.windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::update() {
    eyeAnimator.update();
    light.update();
    if (music) {
        if (eyeAnimator.inGame()) {
            music->engine.start();
            ofVec3f max;
            for (auto& a : eyeAnimator.contours.contourFinder.blobs) {
                if (a.centroid.x > max.x) {
                    max.x = a.centroid.x;
                }
                if (a.centroid.y > max.y) {
                    max.y = a.centroid.y;
                }
                if (a.centroid.z > max.z) {
                    max.z = a.centroid.z;
                }
            }
            if (max.x > 0.0f) {
                float pitch = ofMap(max.x, 0, cameraWidth, 42.0f, 72.0f);
                music->pitch_ctrl.set(pitch);
            }
            else {
                //music->pitch_ctrl.set(ofRandom(36.0f, 72.0f));
            }
            if (max.y > 0.0f) {
                float amp = ofMap(max.y, 0, cameraHeight, 1.0f, 0.5f);
                music->amp_ctrl.set(amp);
            }
        }
        else {
            music->engine.stop();
        }
        music->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetBackgroundColor(ofColor::black);
    ofSetColor(ofColor::white);
    ofLogNotice() << "draw ";
    if (hideMenu) {
        ofEnableDepthTest();
        ofPushStyle();
        ofPushMatrix();
        light.enable();
        //camera.begin();
    // show spirl eye first, if its not running try to text if al else fails show the main eye
        eyeAnimator.draw(music);
        ofPopMatrix();
        ofPopStyle();
       // camera.end();

        light.disable();
        ofDisableDepthTest();
    }
    else  {
        gui.draw();
    }
}
void ofApp::keyReleased(int key) {
}
void ofApp::keyPressed(int key) {
    if (key == 'm') {
        gui.saveToFile("settings.xml");
        hideMenu = !hideMenu;
    }
    else if (key == 's') {
        gui.saveToFile("settings.xml");
        hideMenu = !hideMenu;
    }
    else if (key == 'l') {
        gui.loadFromFile("settings.xml");
    }


    if (!music) {
        return;
    }

    // we can launch our sequences with the launch method, with optional quantization
    /*
    switch (key) {
    case '1':
        music->engine.sequencer.sections[0].launch(0, music->quantize, music->quantime);
        break;
    case '2':
        music->engine.sequencer.sections[0].launch(1, music->quantize, music->quantime);
        break;
    case '3':
        music->engine.sequencer.sections[0].launch(2, music->quantize, music->quantime);
        break;
    case '4':
        music->engine.sequencer.sections[0].launch(3, music->quantize, music->quantime);
        break;
    case '6':
        music->quantize = true;
        music->quantime = 1.0;
        break;
    case '7':
        music->quantize = true;
        music->quantime = 1.0 / 4.0;
        break;
    case '8':
        music->quantize = true;
        music->quantime = 1.0 / 8.0;
        break;
    case '9':
        music->quantize = true;
        music->quantime = 1.0 / 16.0;
        break;
    case '0':
        music->quantize = false;
        break;
    case 'q':
        music->engine.sequencer.sections[1].launch(0, music->quantize, music->quantime);
        break;
    case 'w':
        music->engine.sequencer.sections[1].launch(1, music->quantize, music->quantime);
        break;
    case 'e':
        music->engine.sequencer.sections[1].launch(2, music->quantize, music->quantime);
        break;
    case 'r':
        music->engine.sequencer.sections[1].launch(3, music->quantize, music->quantime);
        break;
    case ' ': // pause / play
        if (music->engine.sequencer.isPlaying()) {
            music->engine.sequencer.pause();
        }
        else {
            music->engine.sequencer.play();
        }
        break;
    case 's': // stop
        music->engine.sequencer.stop();
        break;
    }
    */
  
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
// --------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    if (music) {
        float pitch = ofMap(x, 0, ofGetWidth(), 36.0f, 72.0f);
       // music->pitch_ctrl.set(pitch);
        float amp = ofMap(y, 0, ofGetHeight(), 1.0f, 0.0f);
        //music->amp_ctrl.set(amp);
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if (music) {
        float pitch = ofMap(x, 0, ofGetWidth(), 36.0f, 72.0f);
        //music->pitch_ctrl.set(pitch);
        float amp = ofMap(y, 0, ofGetHeight(), 1.0f, 0.0f);
        //music->amp_ctrl.set(amp);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if (music) {
        float pitch = ofMap(x, 0, ofGetWidth(), 36.0f, 72.0f);
        //music->pitch_ctrl.set(pitch);

        // y value controls the trigger intensity
        float trig = ofMap(y, 0, ofGetHeight(), 1.0f, 0.000001f);
        //music->gate_ctrl.trigger(trig); // we send a trigger to the envelope
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    if (music) {
       // music->gate_ctrl.off(); // we send an "off" trigger to the envelope
    }
    // this is the same as writing
    // gate_ctrl.trigger( 0.0f ); 
}


