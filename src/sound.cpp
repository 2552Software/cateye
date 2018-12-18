#include "ofApp.h"
//https://freewavesamples.com/alesis-fusion-acoustic-bass-c2

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

