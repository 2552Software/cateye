#include "ofApp.h"

#include "sound.h"

// sound https://www.liutaiomottola.com/formulae/freqtab.htm

void Music::update() {

}
void Music::setPixels(ofxCvContourFinder&contours, int max) {

    float x=0.0f, y=0.0f;

    // find largest
    for (auto& blob : contours.blobs) {
        if (blob.boundingRect.x > 1 && blob.boundingRect.y > 1) {  //x,y 1,1 is some sort of strange case
            if (blob.boundingRect.x > x){
                x = blob.boundingRect.x;
            }
            if (blob.boundingRect.y > y) {
                y = blob.boundingRect.y;
            }
        }
    }

    if (x) {
        float pitch = ofMap(x, 0, ofGetWidth(), 36.0f, 72.0f);
        pitch_ctrl.set(pitch);
    }
    if (y) {
        float amp = ofMap(y, 0, ofGetHeight(), 1.0f, 0.0f);
        amp_ctrl.set(amp);
    }

}

void Music::setup(int len, int maxPartials) {

    pitch_ctrl >> synth.in("pitch"); // patching with in("tag")
    amp_ctrl >> synth.in_amp(); // patching with custom in_tag()
    synth * dB(-12.0f) >> engine.audio_out(0);
    synth * dB(-12.0f) >> engine.audio_out(1);


    pitch_ctrl.set(60.0f);
    pitch_ctrl.enableSmoothing(50.0f); // 50ms smoothing
    amp_ctrl.set(0.0f);
    amp_ctrl.enableSmoothing(50.0f); // 50ms smoothing

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
    // engine.sequencer.sections[1].out_value(1).enableSmoothing(100.0f);

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

    engine.listDevices();
    engine.setDeviceID(0); // REMEMBER TO SET THIS AT THE RIGHT INDEX!!!!
    engine.setup(44100, 512, 3);

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


