#include "ofApp.h"

void PolySynth::setup(int numVoices) {
    // -------------------------- PATCHING ------------------------------------
    voices.resize(numVoices);

    for (int i = 0; i < numVoices; ++i) {
        voices[i].setup(*this, i);
    }

    // we filter the frequency below 20 hz (not audible) just to remove DC offsets
    20.0f >> leakDC.in_freq();

    leakDC >> chorus.ch(0) >> gain.ch(0);
    leakDC >> chorus.ch(1) >> gain.ch(1);

    // pdsp::Switch EXAMPLE ---------------------------------------------------
    lfo_switch.resize(2);  // resize input channels
    lfo.out_triangle() >> lfo_switch.input(0); // you cannot use this input() method in a chain
    lfo.out_saw() >> lfo_switch.input(1); // because: API reasons
   //bugbug lfo.out_square() >> lfo_switch.input(2);
   //bugbug lfo.out_sine() >> lfo_switch.input(3);
    //bugbug lfo.out_sample_and_hold() >> lfo_switch.input(4);

    lfo_wave_ctrl >> lfo_switch.in_select(); // input for output selection

    lfo_speed_ctrl >> lfo.in_freq();
    lfo_switch >> lfo_filter_amt;
    // ------------------------------------------------------------------------

    // CONTROLS ---------------------------------------------------------------
    ui.setName("DATASYNTH");

    ui.add(filter_mode_ctrl.set("filter mode", 0, 0, 5));
    ui.add(cutoff_ctrl.set("filter cutoff", 120, 10, 120));
    ui.add(reso_ctrl.set("filter reso", 0.0f, 0.0f, 1.0f));

    cutoff_ctrl.enableSmoothing(200.0f);

    ui.add(env_attack_ctrl.set("env attack", 50, 5, 1200));
    ui.add(env_decay_ctrl.set("env decay", 600, 5, 1200));
    ui.add(env_sustain_ctrl.set("env sustain", 1.0f, 0.0f, 1.0f));
    ui.add(env_release_ctrl.set("env release", 900, 5, 2000));
    ui.add(env_filter_amt.set("env to filter", 30, 0, 60));

    ui.add(lfo_wave_ctrl.set("lfo wave", 0, 0, 4));
    ui.add(lfo_speed_ctrl.set("lfo freq", 0.5f, 0.005f, 4.0f));
    ui.add(lfo_filter_amt.set("lfo to filter", 0, 0, 60));
    // ------------------------------------------------------------------------

    // Chorus -----------------------------------------------------------------
    chorus_speed_ctrl >> chorus.in_speed();
    chorus_depth_ctrl >> chorus.in_depth();
    ui.add(chorus_speed_ctrl.set("chorus freq", 0.5f, 0.25f, 1.0f));
    ui.add(chorus_depth_ctrl.set("chorus depth", 3.5f, 1.0f, 10.0f));
    ui.add(gain.set("gain", -9, -48, 12));
    gain.enableSmoothing(50.f);
    // ------------------------------------------------------------------------
}

void PolySynth::Voice::setup(PolySynth & m, int v) {

    addModuleInput("trig", voiceTrigger);
    addModuleInput("pitch", oscillator.in_pitch());
    addModuleOutput("signal", amp);

    oscillator.setTable(m.datatable);
    // SIGNAL PATH
    oscillator >> filter >> amp >> m.leakDC;

    // MODULATIONS AND CONTROL
    voiceTrigger >> envelope >> amp.in_mod();
    envelope >> m.env_filter_amt.ch(v) >> filter.in_pitch();
    m.lfo_filter_amt >> filter.in_pitch();
    m.cutoff_ctrl >> filter.in_pitch();
    m.reso_ctrl >> filter.in_reso();
    m.filter_mode_ctrl >> filter.in_mode();

    m.env_attack_ctrl >> envelope.in_attack();
    m.env_decay_ctrl >> envelope.in_decay();
    m.env_sustain_ctrl >> envelope.in_sustain();
    m.env_release_ctrl >> envelope.in_release();

}

void Music::setup(int len, int maxPartials) {
    mode = 0;
  
    //patching-------------------------------
   // keyboard.setPolyMode(8);

    int voicesNum = keyboard.getVoicesNumber();

    synth.datatable.setup(len, maxPartials); // as many samples as the webcam width for example
    //synth.datatable.smoothing(0.5f);

    synth.setup(voicesNum);
    for (int i = 0; i < voicesNum; ++i) {
        // connect each voice to a pitch and trigger output
        keyboard.out_trig(i) >> synth.voices[i].in("trig");
        keyboard.out_pitch(i) >> synth.voices[i].in("pitch");
    }
    //------------SETUPS AND START AUDIO-------------
    // patch synth to the engine
    synth.ch(0) >> engine.audio_out(0);
    synth.ch(1) >> engine.audio_out(1);

    // ------------------------- PATCHING -------------------------------------
    seq_mode = 0;
    quantize = true;
    quantime = 1.0 / 8.0; // 1/4th

    engine.sequencer.setTempo(120.0f); // sets the tempo of the music

    engine.sequencer.sections.resize(2);
    // this is very important, each section can play just one sequence at time
    // you have to resize it to a sufficient number before starting to assign sequences

    // each section can output the messages to one or more outputs, as values or triggers
    // thing about them as gate/cv outputs, gate=trig, value=cv
    engine.sequencer.sections[0].out_trig(0) >> kick.in("trig"); // assign the first sequence output to trig
    engine.sequencer.sections[1].out_trig(0) >> lead.in("trig"); // assign the first sequence output to trig
    engine.sequencer.sections[1].out_value(1) >> lead.in("pitch"); // assign the second sequence output to values

    // decomment this to slew the lead value output signal
    engine.sequencer.sections[1].out_value(1).enableSmoothing(100.0f);

    // patching (with panning)
    kick * (dB(-6.0f) * pdsp::panL(-0.25f)) >> engine.audio_out(0);
    kick * (dB(-6.0f) * pdsp::panR(-0.25f)) >> engine.audio_out(1);

    lead * (dB(-6.0f) * pdsp::panL(0.25f)) >> engine.audio_out(0);
    lead * (dB(-6.0f) * pdsp::panR(0.25f)) >> engine.audio_out(1);

    // ------------------------ SEQUENCING ------------------------------------

    // kick sequences
    kick_seqs.resize(4);

    // we can use "set" to use an inlined array with the step values
    // "steplen" and "bars" are used for the timing and lenght of the sequence
    // init step length is 1/16 and init seq lenght is 1 bar
    float o = -1.0f; // when using "set" negative outputs are ignored, 'o' is graphically useful

    // kick synth has an AHR envelope, only positive values will trigger it

    kick_seqs[0].steplen = 1.0 / 4.0;
    kick_seqs[0].bars = 1.0; // this can also be omitted, 1.0 is the default value
    kick_seqs[0].set({ 1.0f, o, o, o });

    kick_seqs[1].steplen = 1.0 / 4.0;
    kick_seqs[1].set({ 1.0f, 0.5f, 0.8f, 0.5f });

    kick_seqs[2].steplen = 1.0 / 8.0;
    kick_seqs[2].set({ 1.0f, 0.5f, o, o,  o, o, 0.3f, o });

    kick_seqs[3].steplen = 1.0 / 16.0;
    kick_seqs[3].bars = 2.0;
    kick_seqs[3].set({ 1.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
                        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f });

    lead_seqs.resize(4);

    // lead sequences ( and sequences examples ) 
    lead_seqs[0].steplen = 1.0 / 16.0;
    lead_seqs[0].set({ { 1.0f,  0.0f,  0.75f,  0.0f,  1.0f,  0.0f, 0.75f, 0.0f }, // triggers outputs
                        { 84.0f, o,     82.0f,  o,     84.0f, o,    80.0f, o    } }); // pitch outputs

    lead_seqs[1].steplen = 1.0 / 16.0;
    lead_seqs[1].set({ { 1.0f,  o,   o,   o,   1.0f,   o,   o,    0.0f },
                        { 72.0f, o,   o,   o,   75.f,   o,   77.f, o    } });


    float minor_penta[] = { 72.0f, 75.0f, 77.0f, 79.0f, 82.0f, 84.0f, 87.0f, 89.0f }; // minor pentatonic scale

    // you can also use begin(), message() and end() to set sequences
    lead_seqs[2].steplen = 1.0 / 8.0;
    lead_seqs[2].begin();
    for (int i = 0; i < 4; ++i) {
        float trig = (i % 2 == 0) ? 1.0f : 0.75f;
        float pitch = minor_penta[pdsp::dice(8)]; // random pitch from the array, pdsp::dice give you an int

        lead_seqs[2].message(double(i), trig, 0); // step, value, output (we set 0 as trig out)
        lead_seqs[2].message(double(i) + 0.1f, 0.0f, 0); // trigger off, short step gate
        lead_seqs[2].message(double(i), pitch, 1); // step, value, output (we set 1 as value out)
        // step value is double so you can use fractional timing if you want
    }
    lead_seqs[2].end(); // always call end() when you've done


    // this is the same as before, but we put the code into a lambda function,
    // the lambda function code is executed each time the sequence is retriggered
    // so each time the sequence is started it generates new patterns
    // read the ofBook about lambdas: https://openframeworks.cc/ofBook/chapters/c++11.html#lambdafunctions

    lead_seqs[3].code = [&]() noexcept { // better to tag noexcept for code used by the DSP engine 
        pdsp::Sequence & seq = lead_seqs[3]; // reference

        // declaring a variable inside the lambda is fine
        static float akebono[] = { 72.0f, 74.0f, 75.0f, 79.0f, 80.0f, 84.0f, 86.0f, 87.0f }; // akebono scale

        seq.steplen = 1.0 / 8.0;
        seq.begin();
        for (int i = 0; i < 4; ++i) {
            float trig = (i % 2 == 0) ? 1.0f : 0.75f;
            float pitch = akebono[pdsp::dice(8)];

            seq.message(double(i), trig, 0);
            seq.message(double(i) + 0.6f, 0.0f, 0); // trigger off, half step gate
            seq.message(double(i), pitch, 1);
        }
        seq.end();
    };

    // assigning the sequences to the sequencer sections
    for (int i = 0; i < 4; ++i) {
        // arguments: cell index, pointer to pdsp:Sequence, behavior (pdsp::Behavior::Loop if not given)
        engine.sequencer.sections[0].setCell(i, kick_seqs[i]);
        engine.sequencer.sections[1].setCell(i, lead_seqs[i], pdsp::Behavior::OneShot);
    }
    //------------SETUPS AND START AUDIO-------------
    engine.listDevices();
    engine.setDeviceID(0); // REMEMBER TO SET THIS AT THE RIGHT INDEX!!!!
    engine.setup(44100, 512, 3);

    return;

    oneShot = false;

    //-------------------------SETTING UP SCORE--------------

    o = 0.0f; // we need at least two values for each dimension to avoid ambigous calls

    bleeps.resize(4);
    for (size_t i = 0; i < bleeps.size(); ++i) {
        bleeps[i].steplen = 1.0 / 16.0;
        bleeps[i].bars = 1.0 / 4.0;
    }
    bleeps[0].set({ {1.0f, o}, {77.0f, o} });
    bleeps[1].set({ {1.0f, o}, {80.0f, o} });
    bleeps[2].set({ {1.0f, o}, {84.0f, o} });
    bleeps[3].set({ {1.0f, o}, {89.0f, o} });

    // SequencerProcessor setup
    engine.sequencer.setTempo(120.0);
    engine.sequencer.setMaxBars(16.0);
    engine.sequencer.sections.resize(2);

    // adding the bleep patterns and settings their timings
    for (size_t i = 0; i < bleeps.size(); ++i) {
        engine.sequencer.sections[0].setCell(i, bleeps[i], pdsp::Behavior::Next);
    }
    engine.sequencer.sections[0].launch(0);

    // set bass sequence
    engine.sequencer.sections[1].setCell(0, bassPattern);
    engine.sequencer.sections[1].launch(0);

    //-------------------------PATCHING--------------
    engine.sequencer.sections[0].out_trig(0) >> bleep.in("trig");
    engine.sequencer.sections[0].out_value(1) >> bleep.in("pitch");

    // connect section outputs to bass
    engine.sequencer.sections[1].out_trig(0) >> bass.in("trig");
    engine.sequencer.sections[1].out_value(1) >> bass.in("pitch");

    engine.sequencer.sections[1].out_value(1).setSlewTime(80.0f); // 50ms slew
    engine.sequencer.sections[1].linkSlewControl(1, 2);
    // now the out 2 will control the slew time of the value output 1
    // slew time is multiplied by the message value
    // so for example 0.0f deativates slew and 2.0f doubles it
    // useful for 303-like glides
    // (the effect is subtle in this example but it's there)

// connect synths to stereo output
    bass * (pdsp::panL(-0.5f) * dB(-6.0f)) >> engine.audio_out(0);
    bass * (pdsp::panR(-0.5f) * dB(-6.0f)) >> engine.audio_out(1);

    bleep * (pdsp::panL(0.5f) * dB(-6.0f)) >> engine.audio_out(0);
    bleep * (pdsp::panR(0.5f) * dB(-6.0f)) >> engine.audio_out(1);

}
void SuperSphere::setup(const string&name) {
    eye.setup(name);
    setResolution(21);
    panDeg(180);
}
void TextTimer::setup() {
}

void Eye::setup(const string&texName) {
    if (ofLoadImage(*this, texName)) {
        ofLogNotice("Eye") << "loaded " << texName;
    }
    else {
        ofLogError("Eye") << "not loaded " << texName;
    }
}

GameItem::GameItem(const ofRectangle& rect, Eye eye, Levels levelIn, int idIn) {
    id = idIn;
    rectangle = rect;
    level = levelIn;
    myeye = eye;
    setup();
}

void GameItem::setup() {

    animater.reset(0.0f);
    animater.setCurve(EASE_IN_EASE_OUT);
    animater.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    float duration=20.0f;

    if (level == EndGame) {
        duration = 60.0f; //this is the big prize, enjoy it
    }
    else if (level == Difficult) {
        cylinder.setRadius(min(rectangle.height, rectangle.width) / 2);
        cylinder.setHeight(10); // thin
        // use with squares etc sphere.set(rect.width, rect.height, 0.0f);
        cylinder.setPosition(rectangle.x, rectangle.y, 0.0f);
        duration = 60.0f; 
    }
    else if (level == Medium) {
        box.setPosition(rectangle.x, rectangle.y + rectangle.height, -2 * rectangle.width);
        box.setWidth(rectangle.width);
        box.setHeight(rectangle.height);
        duration = 20.0f;
    }
    else if (level == Basic) {
        sphere.setRadius(min(rectangle.height, rectangle.width) / 2);
        // use with squares etc sphere.set(rect.width, rect.height, 0.0f);
        sphere.setPosition(rectangle.x, rectangle.y, 0.0f);
        duration = 30.0f;
    }
    animater.setDuration(duration);
    animater.animateTo(1.0f);
}

void Eyes::setup(AnimRepeat repeat, float seconds, const std::string& path, float rotateIn) {
    rotate = rotateIn;

    getAnimator().reset(0.0f);
    getAnimator().setDuration(seconds);
    getAnimator().setRepeatType(repeat);
    getAnimator().setCurve(LINEAR);

    ofDirectory dir(path);
    dir.allowExt("png");
    dir.allowExt("jpg");
    size_t i = 0;
    if (dir.listDir() > 0) {
        selector.reset(0.0f);
        selector.setDuration(60.0f); // do not pick too often bugbug menu
        selector.setRepeatType(LOOP_BACK_AND_FORTH);
        selector.setCurve(LINEAR);
        selector.animateTo(dir.size()-1);
        for (; i < dir.size(); i++) {
            add(dir.getName(i), dir.getPath(i));
        }
    }
    else {
        ofLogError() << path << " missing"; // fatal?
    }
}

void ContoursBuilder::setup() {
    vector<ofVideoDevice> devices = video.listDevices();
    bool found = false;
    for (auto& device : devices) {
        ofLogVerbose("ContoursBuilder::setup") << device.deviceName << " " << device.id;
        if (device.deviceName.find("facetime") == std::string::npos) { // if its not face time use it
            video.setDeviceID(device.id);
            ofLogNotice("ContoursBuilder::setup found ") << device.deviceName;
            found = true;
            break;
        }
    }
    if (!found) {
        ofLogFatalError("ContoursBuilder::setup no device found");
        ofExit(1);
    }
    video.setVerbose(true);
    video.setPixelFormat(OF_PIXELS_RGB);
    video.setDesiredFrameRate(15);
    video.setup(cameraWidth, cameraHeight, false);
    colorImg.allocate(cameraWidth, cameraHeight);
    grayImage.allocate(cameraWidth, cameraHeight);
    grayDiff.allocate(cameraWidth, cameraHeight);
    ofLogNotice("ContoursBuilder::setup over ");
}

void ImageAnimator::setup() {

    level = NoGame;
    // convert to screen size
    xFactor = ofGetScreenWidth() / cameraWidth;
    yFactor = ofGetScreenHeight() / cameraHeight;

    music.setup(cameraWidth, cameraHeight); // tie to app
    font.load("DejaVuSans.ttf", 100, true, true, true);
    font.setLineHeight(18.0f);
    font.setLetterSpacing(1.037);

    ofAddListener(rotatingEyes.getAnimator().animFinished, this, &ImageAnimator::rotatingEyesDone);
    ofAddListener(textFinished, this, &ImageAnimator::creditsDone);

    buildTable();
    buildX();
    buildY();

    mainEyes.setup(PLAY_ONCE, 1.0f, EYES, 0.0f);
    if (!mainEyes.count()) {
        ofExit(100);
    }

    rotatingEyes.setup(LOOP_BACK_AND_FORTH_ONCE, 3.0f, SPIRALS, 25.0f);
    cubes.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, CUBES, 25.0f);
    spheres.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, SPHERES, 25.0f);
    musicNotes.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, MUSICNOTES, 25.0f);
    cylinders.setup(LOOP_BACK_AND_FORTH_ONCE, 1.0f, CYLINDERS, 25.0f);

    contours.setup();

    //ofDirectory allSounds(SOUNDS);
    //allSounds.allowExt("wav");
   // allSounds.allowExt("mp3");
   // allSounds.listDir();
    //for (size_t i = 0; i < allSounds.size(); i++) {
      //  ofSoundPlayer sound;
       //bugbug not working on pi but replace with the cools stguf any sound.load(allSounds.getPath(i));
        //mySounds.push_back(sound);
   // }

    blinker.reset(0.0f);
    blinker.setCurve(EASE_IN_EASE_OUT);
    blinker.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
    blinker.setDuration(1.0f);
    blinker.animateTo(1.0f);

    clear(); // go to a known state (call last like this as it may depend on othe settings)
    startPlaying();

    ofLogNotice("ImageAnimator::setup") << "finshed";
}
