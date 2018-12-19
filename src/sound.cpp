#include "ofApp.h"
//https://freewavesamples.com/alesis-fusion-acoustic-bass-c2

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

