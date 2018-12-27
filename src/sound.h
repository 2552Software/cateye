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

// to create your own modules, you have to extend pdsp::Patchable

class SlideSynth : public pdsp::Patchable {

public:

    SlideSynth() { patch(); } // default constructor
    SlideSynth(const SlideSynth & other) { patch(); } // you need this to use std::vector with your class, otherwise will not compile
    // remember that is a bad thing to copy construct in pdsp, 
    //      always just resize the vector and let the default constructor do the work
    //          resizing the vector will also disconnect everything, so do it just once before patching


    void patch() {

        //create inputs/outputs to be used with the in("tag") and out("tag") methods
        addModuleInput("pitch", osc.in_pitch()); // the first input added is the default input
        addModuleInput("amp", y_ctrl);
        addModuleOutput("signal", amp); // the first output added is the default output


        // pdsp::PatchNode is a class that simply route its inputs to its output
        y_ctrl.enableBoundaries(0.0f, 1.0f); // you can clip the input of pdsp::PatchNode
        y_ctrl.set(0.0f); // and you can set its input default value

        //patching
        osc.out_saw() * 2.0f >> drive >> filter >> amp;
        y_ctrl >> amp.in_mod();
        y_ctrl * 60.0f >> filter.in_cutoff();
        48.0f >> filter.in_cutoff();
        0.3f >> filter.in_reso();
    }

    // those are optional
    pdsp::Patchable & in_pitch() {
        return in("pitch");
    }

    pdsp::Patchable & in_amp() {
        return in("amp");
    }

    pdsp::Patchable & out_signal() {
        return out("signal");
    }

private:

    pdsp::PatchNode     y_ctrl;
    pdsp::PatchNode     pitch_ctrl;
    pdsp::Amp           amp;
    pdsp::VAOscillator  osc;
    pdsp::Saturator1    drive; // distort the signal
    pdsp::VAFilter      filter; // 24dB multimode filter

};

class SineBleep : public pdsp::Patchable {
public:
    SineBleep() { patch(); }
    SineBleep(const SineBleep & other) { patch(); }

    void patch() {
        //add inputs / outputs with these methods 
        addModuleInput("trig", env.in_trig()); // arguments are tag and the Unit in/out to link to that tag
        addModuleInput("pitch", osc.in_pitch());
        addModuleOutput("signal", amp); // if in/out is not selected default in/out is used

        //patching
        env.set(0.0f, 50.0f, 1.0f, 350.0f) * 0.25f >> amp.in_mod();
        env * 0.10f >> osc.in_fb() >> amp;
    }
private:
    pdsp::Amp amp;
    pdsp::FMOperator osc;
    pdsp::ADSR env;

};


class KickSynth : public pdsp::Patchable {
public:
    KickSynth() { patch(); }
    KickSynth(const KickSynth & other) { patch(); }

    void patch() {
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
private:
    pdsp::Amp           amp;
    pdsp::FMOperator    osc;
    pdsp::AHR           ampEnv;
    pdsp::AHR           modEnv;
    pdsp::PatchNode     trigger_in;
};


class Music {
public:
    void setup(int len, int maxPartials);
    void update();
    
    void set(float pitch, float trigger, float amp);

    // pdsp modules
    pdsp::Engine            engine;
    pdsp::VAOscillator      osc;
    pdsp::LFO               lfo;
    pdsp::Amp               amp;
    pdsp::ADSR              env;

    pdsp::TriggerControl    gate_ctrl;
    pdsp::ValueControl      amp_ctrl;
    pdsp::ValueControl      pitch_ctrl;
    SlideSynth              synth;
    pdsp::LowCut            leakDC;
    pdsp::ComputerKeyboard  keyboard; // simulate bugbug
    SineBleep               lead;
    KickSynth               kick;

    std::vector<pdsp::Sequence>  lead_seqs;
    std::vector<pdsp::Sequence>  kick_seqs;

    int                     seq_mode;
    std::atomic<bool>       quantize;
    std::atomic<double>     quantime;
private:

};



