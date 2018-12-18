#include "ofApp.h"

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

