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

class Music {
public:
    Music(float f = 172.0f, float v = 0.1f) { frequency = f; volume = v; }
    float frequency = 172.0f;
    float volume = 0.1f;
};

class Sound {
public:
    void setup();
    void update();

    float volume;
    float frequency;
    // pdsp modules
    pdsp::Engine            engine;
    pdsp::VAOscillator      osc;
    pdsp::LFO               lfo;
    // pdsp modules
    SineBleep               lead;
    KickSynth               kick;
    std::vector<pdsp::Sequence>  lead_seqs;
    std::vector<pdsp::Sequence>  kick_seqs;
    int                     seq_mode;
    std::atomic<bool>       quantize;
    std::atomic<double>     quantime;

private:

    float frequencyTarget; // remove old stuff when brave enough
    float volumnTarget;

    double wavePhase;
    double pulsePhase;

    std::mutex audioMutex;
    ofSoundStream soundStream;
    ofSoundBuffer lastBuffer;
    ofPolyline waveform;
    float rms;
};

