#include "ofApp.h"

#include "sound.h"

void Music::update() {

}
void Music::setPixels(ofxCvContourFinder&contours) {
    if (contours.blobs.size() > 0) {
        size_t c = contours.blobs.size();
        switch (mode) {
        case 0: // converting pixels to waveform samples
            synth.datatable.begin();
            // first blob is the biggest, take that blob and use every 3rd point, if that is too much data?
            for (size_t n = 0; n < cameraWidth; ++n) {
                float sample = 0.0f;
                if (n < c) {
                    sample = contours.blobs[n].boundingRect.x + contours.blobs[n].boundingRect.width +
                        contours.blobs[n].boundingRect.height + contours.blobs[n].boundingRect.y;
                }
                synth.datatable.data(n, ofMap(sample, 0, cameraWidth + cameraHeight, -0.5f, 0.5f));
            }
            synth.datatable.end(false);
            break; // remember, raw waveform could have DC offsets, we have filtered them in the synth using an hpf

        case 1: // converting pixels to partials for additive synthesis
            synth.datatable.begin();
            for (size_t n = 0; n < cameraWidth; ++n) {
                float partial = 0.0f;
                if (n < c) {
                    partial = contours.blobs[n].boundingRect.x + contours.blobs[n].boundingRect.width +
                        contours.blobs[n].boundingRect.height + contours.blobs[n].boundingRect.y;
                }
                partial = ofMap(partial, 0, cameraWidth*cameraHeight, 0.0f, 1.5f);
                synth.datatable.data(n, partial);
            }
            synth.datatable.end(true);
            break;
        }
    }
}

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


//https://freewavesamples.com/alesis-fusion-acoustic-bass-c2

/* common frequencies 

        float freqsLow[] = {
            82.407, 87.307f, 92.499f, 97.999f, 103.826f, 110.0f, 116.541f, 123.471f, 130.813f, 138.591f, 146.8325f,155.563f, 164.814, 174.614, 184.997,
            195.998, 207.652, 220.0, 233.082,246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.0,466.164,
            493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 880.0, 932.328,  987.767, 1046.502, 1108.731, 1174.659 };

        float freqsHigh[] = {
            1244.50, 1318.51,1396.913,1479.978,1567.982,1661.219,1760.0, 1864.655, 1975.533,2093.005,2217.461,2349.318,2489.016,2637.021,2793.826,
            2793.826,2959.955, 3135.964,3322.438,3520.0, 3729.31,3951.066,4186.009,4434.922,4698.636,4978.032,5274.042,5587.652, 5919.91,6271.92,6644.876,7040.0,
            7458.62,7902.132,8372.018,8869.844,9397.272,9956.064,10548.084,11175.304, 11839.82, 12543.856, 12543.856, 13289.752, 14917.24, 15804.264
        };

*/

ofParameterGroup & WaveSynth::setup(int numVoices, float spread, std::string name) {

    // -------------------------- PATCHING ------------------------------------
    voices.resize(numVoices);

    for (int i = 0; i < numVoices; ++i) {
        // setup voice
        voices[i].setup(*this);

        if (spread == 0.0f) {
            voices[i] >> gain.ch(0);
            voices[i] >> gain.ch(1);
        }
        else {
            float spreadamt = pdsp::spread(i, numVoices, spread);
            voices[i] * pdsp::panL(spreadamt) >> gain.ch(0);
            voices[i] * pdsp::panR(spreadamt) >> gain.ch(1);
        }
    }

    // CONTROLS ---------------------------------------------------------------
    parameters.setName(name);
    parameters.add(table_ctrl.set("table index", 8.0f, 0.0f, (float)(wavetable.size() - 1)));
    table_ctrl.enableSmoothing(200.0f);

    parameters.add(filter_mode_ctrl.set("filter mode", 0, 0, 5));
    parameters.add(cutoff_ctrl.set("filter cutoff", 82, 10, 120));
    parameters.add(reso_ctrl.set("filter reso", 0.0f, 0.0f, 1.0f));

    cutoff_ctrl.enableSmoothing(200.0f);

    parameters.add(env_attack_ctrl.set("env attack", 400, 5, 1200));
    parameters.add(env_decay_ctrl.set("env decay", 400, 5, 1200));
    parameters.add(env_sustain_ctrl.set("env sustain", 1.0f, 0.0f, 1.0f));
    parameters.add(env_release_ctrl.set("env release", 900, 5, 2000));
    parameters.add(env_table_ctrl.set("env to table", 0.0f, 0.0f, 2.0f));
    parameters.add(env_filter_ctrl.set("env to filter", 30, 0, 60));
    parameters.add(drift.set("drift", 0.0f, 0.0f, 1.0f));

    parameters.add(gain.set("gain", -12, -48, 12));
    gain.enableSmoothing(50.f);

    return parameters;
}


void WaveSynth::Voice::setup(WaveSynth & m) {

    addModuleInput("trig", voiceTrigger);
    addModuleInput("pitch", oscillator.in_pitch());
    addModuleInput("table", oscillator.in_table());
    addModuleInput("cutoff", filter.in_pitch());
    addModuleOutput("signal", voiceAmp);

    oscillator.setTable(m.wavetable);

    // SIGNAL PATH
    oscillator >> filter >> voiceAmp;

    // MODULATIONS AND CONTROL
    envelope >> envToTable >> oscillator.in_table();
    m.table_ctrl >> oscillator.in_table();

    voiceTrigger >> envelope >> voiceAmp.in_mod();
    envelope >> envToFilter >> filter.in_pitch();
    m.cutoff_ctrl >> filter.in_pitch();
    m.reso_ctrl >> filter.in_reso();
    m.filter_mode_ctrl >> filter.in_mode();


    m.env_attack_ctrl >> envelope.in_attack();
    m.env_decay_ctrl >> envelope.in_decay();
    m.env_sustain_ctrl >> envelope.in_sustain();
    m.env_release_ctrl >> envelope.in_release();

    m.env_filter_ctrl >> envToFilter.in_mod();
    m.env_table_ctrl >> envToTable.in_mod();

    0.2f >> phazorFree;
    0.05f >> randomSlew.in_freq();
    m.drift >> driftAmt.in_mod();
    phazorFree.out_trig() >> rnd >> randomSlew >> driftAmt;
    driftAmt >> oscillator.in_pitch();
    driftAmt * 7.0f >> filter.in_pitch();
    driftAmt >> oscillator.in_table();
}

float WaveSynth::Voice::meter_mod_env() const {
    return envelope.meter_output();
}

float WaveSynth::Voice::meter_pitch() const {
    return oscillator.meter_pitch();
}

pdsp::Patchable& WaveSynth::ch(size_t index) {
    pdsp::wrapChannelIndex(index);
    return gain.ch(index);
}

pdsp::Patchable& WaveSynth::Voice::in_trig() {
    return in("trig");
}

pdsp::Patchable& WaveSynth::Voice::in_pitch() {
    return in("pitch");
}
pdsp::Patchable& WaveSynth::Voice::in_table() {
    return in("table");
}

pdsp::Patchable& WaveSynth::Voice::in_cutoff() {
    return in("cutoff");
}

ofParameterGroup & WaveSynth::label(std::string name) {
    parameters.setName(name);
    return parameters;
}

int AudioPlayer::number = 0;

void AudioPlayer::patch() {

    addModuleOutput("0", fader0);
    addModuleOutput("1", fader1);

    pitchControl >> sampler0.in_pitch();
    pitchControl >> sampler1.in_pitch();

    sampleTrig >> sampler0 >> amp0;
    envGate >> env >> amp0.in_mod();
    sampleTrig >> sampler1 >> amp1;
    env >> amp1.in_mod();

    sampler0 >> amp0 >> fader0;
    sampler1 >> amp1 >> fader1;

    faderControl >> dBtoLin >> fader0.in_mod();
    dBtoLin >> fader1.in_mod();

    sampler0.addSample(&sample, 0);
    sampler1.addSample(&sample, 1);

    smoothControl >> env.in_attack();
    smoothControl >> env.in_release();

    ui.setName("pdsp player " + ofToString(++number));
    ui.add(faderControl.set("volume", 0, -48, 24));
    ui.add(loadButton.set("load", false));
    ui.add(sampleName.set("sample", "no sample"));
    ui.add(samplePath.set("path", "no path"));
    ui.add(pitchControl.set("pitch", 0, -24, 24));
    ui.add(smoothControl.set("fade ms", 0, 0, 50));
    ui.add(bPlay.set("play", false));
    ui.add(bPause.set("pause", false));
    ui.add(bStop.set("stop", true));

    loadButton.addListener(this, &AudioPlayer::loadButtonCall);
    samplePath.addListener(this, &AudioPlayer::sampleChangedCall);
    bPlay.addListener(this, &AudioPlayer::onPlay);
    bPause.addListener(this, &AudioPlayer::onPause);
    bStop.addListener(this, &AudioPlayer::onStop);

    bSemaphore = true;
    sample.setVerbose(true);
}

void AudioPlayer::onPlay(bool & value) {

    if (bSemaphore) {
        bSemaphore = false;
        if (bStop) {
            bPlay = true;
            bStop = false;
            envGate.trigger(1.0f);
            sampleTrig.trigger(1.0f);
            ofLogVerbose() << "[pdsp] player: playing\n";
        }
        else if (bPause) {
            ofLogVerbose() << "[pdsp] player: unpaused\n";
            bPlay = true;
            bPause = false;
            envGate.trigger(1.0f);
        }
        else {
            bPlay = true;
            sampleTrig.trigger(1.0f);
        }
        bSemaphore = true;
    }

}

void AudioPlayer::onPause(bool & value) {

    if (bSemaphore) {
        bSemaphore = false;
        if (bPlay) {
            bPause = true;
            bPlay = false;
            ofLogVerbose() << "[pdsp] player: paused\n";
            envGate.off();
        }
        else if (bStop) {
            bPause = false;
            ofLogVerbose() << "[pdsp] player: impossible to pause on stop";
        }
        else {
            ofLogVerbose() << "[pdsp] player: unpaused\n";
            bPlay = true;
            bPause = false;
            envGate.trigger(1.0f);
        }
        bSemaphore = true;
    }
}

void AudioPlayer::onStop(bool & value) {

    if (bSemaphore) {
        bSemaphore = false;
        if (bPlay || bPause) {
            bStop = true;
            bPlay = false;
            bPause = false;
            ofLogVerbose() << "[pdsp] player: stopped\n";
            envGate.off();
        }
        bSemaphore = true;
    }
}

void AudioPlayer::loadButtonCall(bool & value) {
    if (value) {

        float fvalue = faderControl.get();
        faderControl.set(0.0f);

        //Open the Open File Dialog
        ofFileDialogResult openFileResult = ofSystemLoadDialog("select an audio sample");

        //Check if the user opened a file
        if (openFileResult.bSuccess) {

            string path = openFileResult.getPath();

            samplePath = path;

            ofLogVerbose("file loaded");

        }
        else {
            ofLogVerbose("User hit cancel");
        }

        // switch to mono if the sample has just one channel
        if (sample.channels == 1) {
            sampler1.setSample(&sample, 0, 0);
        }
        else {
            sampler1.setSample(&sample, 0, 1);
        }

        loadButton = false;

        faderControl.set(fvalue);

        bool dummy = true;
        onStop(dummy);
    }
}

void AudioPlayer::sampleChangedCall(string & value) {

    ofLogVerbose("loading" + value);
    loadSample(samplePath);

    auto v = ofSplitString(samplePath, "/");
    sampleName = v[v.size() - 1];
}

void AudioPlayer::loadSample(string path) {
    sample.load(path);
}

void AudioPlayer::load(string path) {
    samplePath = path;
}

void AudioPlayer::play() {
    bPlay = bPlay ? false : true;
}

void AudioPlayer::pause() {
    bPause = bPause ? false : true;
}

void AudioPlayer::stop() {
    bStop = bStop ? false : true;
}


float AudioPlayer::playhead() const {
    return sampler1.meter_position();
}
void Music::smoothCall(float & value) {
    synth.datatable.smoothing(value);
}

float PolySynth::Voice::meter_mod_env() const {
    return envelope.meter_output();
}

float PolySynth::Voice::meter_pitch() const {
    return oscillator.meter_pitch();
}

pdsp::Patchable& PolySynth::ch(int index) {
    index = index % 2;
    return gain.ch(index);
}

BassPattern::BassPattern() {

    sequence = { 29.0f, 31.f, 34.f, 36.f, 38.f, 41.f, 43.f, 29.f };

    code = [&]() noexcept {
        if (counter() == 4) resetCount();
        // pdsp::Sequence has an internal counter() 
        // to count how many times the sequence restarted 
        // that you can reset it with resetCount() 
        // this counter is automatically reset on Sequence change or launch

        shuffleSequence();

        steplen = 1.0 / 16.0;

        begin();

        //   step   velo    pitch       slew%    duration
        note(0.0, 1.0f, 29.0f, 0.0f, gate_long);
        note(2.0, 0.5f, pfun(0), 0.0f, gate_long);
        note(4.0, 0.5f, pfun(1), 1.0f, gate_short);
        note(6.0, 1.0f, pfun(2), 0.0f, gate_long);
        note(8.0, 0.5f, pfun(3), 2.0f, gate_long);
        note(10.0, 1.0f, pfun(4), 0.0f, gate_short);
        note(11.0, 0.5f, pfun(5), 0.0f, gate_short);
        note(12.0, 0.5f, pfun(6), 0.0f, gate_short);
        note(13.0, 0.5f, pfun(7), 0.0f, gate_short);

        end();

    };

}

void BassPattern::note(double step16, float gate, float pitch, float slew, double duration) {
    message(step16, gate, 0); // adds a trigger on to the gate output
    message(step16, pitch, 1); // adds a value to the pitch output
    message(step16, slew, 2); // adds a trigger on to the gate output
    message(step16 + duration, 0.0f, 0);  // adds a trigger off value to the gate output
}

float BassPattern::pfun(int index) {
    if (index > 4 && counter() == 3) {
        float nextPitch = static_cast<float> (pdsp::dice(12) + 41.0f);
        return nextPitch;
    }
    else {
        return sequence[index];
    }
}
void BassPattern::shuffleSequence() {
    int seqLen = static_cast<int> (sequence.size());
    int index1 = pdsp::dice(seqLen);
    int index2 = index1 + pdsp::dice(seqLen - 1);
    while (index2 >= seqLen) { index2 -= seqLen; }
    float temp = sequence[index1];
    sequence[index1] = sequence[index2];
    sequence[index2] = temp;
}
void MultiSampler::add(string path, bool setHoldTime) {
    samples.push_back(new pdsp::SampleBuffer());
    samples.back()->load(path);
    sampler.addSample(samples.back());
}

void MultiSampler::patch() {

    addModuleInput("trig", triggers);
    addModuleInput("position", sampler.in_start());
    addModuleInput("pitch", sampler.in_pitch());
    addModuleInput("select", sampler.in_select());
    addModuleOutput("signal", amp);

    env.enableDBTriggering(-24.0f, 0.0f);
    setAHR(0.0f, 20.0f, 100.0f); // all the samples used here are shorter than this

    triggers >> sampler >> amp;
    triggers >> env >> amp.in_mod();

    samples.reserve(32);
}


void SineBleep::patch() {
    //add inputs / outputs with these methods 
    addModuleInput("trig", env.in_trig()); // arguments are tag and the Unit in/out to link to that tag
    addModuleInput("pitch", osc.in_pitch()); 
    addModuleOutput("signal", amp); // if in/out is not selected default in/out is used

    //patching
    env.set(0.0f, 50.0f, 1.0f, 350.0f) * 0.25f >> amp.in_mod();
    env * 0.10f >> osc.in_fb() >> amp;
}

void KickSynth::patch() {
    //set inputs/outputs
    addModuleInput("trig", trigger_in);
    addModuleOutput("signal", amp);

    //patching
    osc >> amp;
    trigger_in >> ampEnv.set(0.0f, 50.0f, 100.0f) >> amp.in_mod();
    trigger_in >> modEnv.set(0.0f, 0.0f, 50.0f) * 48.0f >> osc.in_pitch();
    48.0f >> osc.in_pitch();
    ampEnv.enableDBTriggering(-24.0f);
}

void BassSynth::patch() {
    //set inputs/outputs
    addModuleInput("trig", trigger_in);
    addModuleInput("pitch", osc.in_pitch());
    addModuleOutput("signal", amp);

    //patching
    osc.out_saw() * 2.5f >> drive >> filter >> amp;

    trigger_in >> ampEnv.set(0.0f, 50.0f, 1.0f, 200.0f) * 0.7f >> amp.in_mod();
    trigger_in >> filterEnv.set(0.0f, 80.0f, 0.0f, 200.0f) * 60.0f >> filter.in_cutoff();
    50.0f >> filter.in_cutoff();
    0.2f >> filter.in_reso();
}

void Reese::patch() {

    addModuleInput("trig", env.in_trig());
    addModuleInput("pitch", pitchNode);
    addModuleOutput("signal", amp);

    osc1.out_saw() * dB(12.0f) >> drive >> filter >> amp;
    osc2.out_saw() * dB(12.0f) >> drive;

    65.0f >> filter.in_cutoff();
    0.1f >> filter.in_reso();

    env.enableDBTriggering(-24.0f, 0.0f);
    env.set(40.0f, 100.0f, 1.0f, 200.0f) >> amp.in_mod();

    pitchNode >> osc1.in_pitch();
    -0.30f >> osc1.in_pitch();
    0.30f >> osc2.in_pitch();
    pitchNode >> osc2.in_pitch();
}

