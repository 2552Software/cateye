#pragma once

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4554 ) 
#pragma warning( disable : 4083 ) 
#endif

#include "ofxPDSP.h"

#ifdef _MSC_VER
#pragma warning( pop )
#endif

struct BassPattern : public pdsp::Sequence {

    // helper routine to add notes to the score
    // this routin also add a message for stopping the note
    // so we have to be careful that notes durations don't overlap
    void note(double step16, float gate, float pitch, float slew, double duration);

    // this routine shuffles the pitches inside of the sequence vector
    void shuffleSequence();

    // this returns the pitches for the generative routine
    // returns the pitches from the sequence the first, second and third bar
    // on the fourth bar the second part of the returned pitches will be random values
    // counter() returns the value of an internal counter that measure how many time the sequence restarted
    float pfun(int index);
    //inits the pattern and set the pitches to use
    BassPattern();
    const double gate_long = 0.95;  // a bit more than 1/16       
    const double gate_short = 0.4; // almost 1/32th
    std::vector<float> sequence;
};

class SineBleep : public pdsp::Patchable {
public:
    SineBleep() { patch(); }
    SineBleep(const SineBleep & other) { patch(); }

    void patch();
private:
    pdsp::Amp amp;
    pdsp::FMOperator osc;
    pdsp::ADSR env;
};

class KickSynth : public pdsp::Patchable {
public:
    KickSynth() { patch(); }
    KickSynth(const KickSynth & other) { patch(); }

    void patch();
private:
    pdsp::Amp           amp;
    pdsp::FMOperator    osc;
    pdsp::AHR           ampEnv;
    pdsp::AHR           modEnv;
    pdsp::PatchNode     trigger_in;
};

class BassSynth : public pdsp::Patchable {
public:
    BassSynth() { patch(); }
    BassSynth(const BassSynth & other) { patch(); }

    void patch();
private:

    pdsp::Amp           amp;
    pdsp::VAOscillator  osc;
    pdsp::VAFilter     filter;
    pdsp::ADSR          ampEnv;
    pdsp::ADSR          filterEnv;
    pdsp::Saturator1    drive;
    pdsp::PatchNode     trigger_in;
};

class Reese : public pdsp::Patchable {
public:
    Reese() { patch(); }
    Reese(const Reese & other) { patch(); }

    void patch();
private:
    pdsp::PatchNode     pitchNode;
    pdsp::VAOscillator  osc1;
    pdsp::VAOscillator  osc2;
    pdsp::Saturator1    drive;
    pdsp::VAFilter      filter;
    pdsp::ADSR          env;
    pdsp::Amp           amp;

};

class MultiSampler : public pdsp::Patchable{

public:
    MultiSampler() { patch(); }
    MultiSampler(const MultiSampler & other) { patch(); }

    ~MultiSampler() { // deallocate elements
        for (size_t i = 0; i < samples.size(); ++i) delete samples[i];
    }

    void patch();

    void add(string path, bool setHoldTime = false);

    void gain(float dBvalue) {       sampler * dB(dBvalue) >> amp;    }

    float meter_env() const {       return env.meter_output();    }

    float meter_position() const {      return sampler.meter_position();    }

    void setAHR(float attack, float hold, float release) {      env.set(attack, hold, release);    }

private:
    pdsp::PatchNode     triggers;
    pdsp::Sampler       sampler;
    pdsp::AHR           env;
    pdsp::Amp           amp;

    std::vector<pdsp::SampleBuffer*> samples;
};

class Music {
public:
    void setup();
    void update();

    float volume;
    float frequency;
    // pdsp modules
    pdsp::Engine            engine;
    pdsp::VAOscillator      osc;
    pdsp::LFO               lfo;

    bool            oneShot;
    BassPattern     bassPattern;
    BassSynth       bass;
    // pdsp modules
    SineBleep       bleep;
    SineBleep       lead;
    KickSynth       kick;
    std::vector<pdsp::Sequence>  lead_seqs;
    std::vector<pdsp::Sequence>  kick_seqs;
    std::vector<pdsp::Sequence> bleeps;
    int                     seq_mode;
    std::atomic<bool>       quantize;
    std::atomic<double>     quantime;
    MultiSampler    drums;
    Reese           reese;

    std::vector<int>     states;

    pdsp::Scope drumScope;
    pdsp::Scope reeseScope;

private:

};



