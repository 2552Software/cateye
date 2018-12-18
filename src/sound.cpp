#include "ofApp.h"
//https://freewavesamples.com/alesis-fusion-acoustic-bass-c2

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
    lfo_switch.resize(5);  // resize input channels
    lfo.out_triangle() >> lfo_switch.input(0); // you cannot use this input() method in a chain
    lfo.out_saw() >> lfo_switch.input(1); // because: API reasons
    lfo.out_square() >> lfo_switch.input(2);
    lfo.out_sine() >> lfo_switch.input(3);
    lfo.out_sample_and_hold() >> lfo_switch.input(4);

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
    ui.add(env_decay_ctrl.set("env decay", 400, 5, 1200));
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

