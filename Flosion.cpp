#include <Flosion/UI/Core/FlosionUI.hpp>

// TODO: oscilloscope

// TODO: it should not be possible to pass an Uncontrolled sound source to an OutOfSync sound input

// TODO: D O C U M E N T A T I O N
// Use Doxygen to better keep documentation consisten with the code itself
// See http://www.doxygen.nl/

// TODO: smoothing options for Resampler (none / linear / cubic)

// TODO: use std::false_type_v<T> for static_asserts inside templates that should always fail

// TODO: PulseWave (square wave with adjustable pulse width, average amplitude always remains zero)

// TODO: make objects created via PanelContextMenu appear
// centered where PanelContextMenu was made (i.e where mouse was double-clicked)

// TODO: find a safe way and clean to delay the initial state allocation
// by Uncontrolled (monostate) objects. State should not be allocated
// until the most-derived class has been constructed, since states may
// refer to this class (i.e. MelodyState, when querying note overlap).
// CRTP may be useful here.

// TODO: find a safe and clean way to ensure that sound/number sources
// contained by UI objects are constructed before base classes are given
// pointers to these from a derived class that is not yet fully constructed.

// TODO: when a SoundNode receives the time of a node upstream, it should
// actually see that upstream node's time, not its own.

// TODO: make volumeToAmp less sensitive

// TODO: Test whether changing flow direction is broken. This may have been an artifact of bugs in transitions that were fixed

// TODO:
// - save everything to a file (ctrl-s)
// - export selection to a file (ctrl-e)
// - clear everything and load from a file (ctrl-o)
// - import something from a file (ctrl-i)

// TODO: allow borrowers to be serialized

// - add Amplifier object
// - add disabling to ui, disable labels
// - add length numbersource to AudioClip and make it global
// - add repeater?
// - add recording to DAC (and maybe separate offline recorder in future)

// TODO: 04/03/2020
// - serialization (note: use typeid to go from instance -> id, and use existing factory macro to go from id -> instance)
// - live input (microphone)
// - looper/live mix thing (uncontrolled; use global keyboard commands to interact with)
// - UI for live melody
// - UI for melody?

// TODO: hot swapping

// TODO: review GUI things where elements are moved between containers
// (such as when borrowers are attached to soundsources) and make use of
// new UI guarantees about event listeners remaining attached

// TODO: remove "std::" where it is not needed thanks to ADL, such as in
// `find(begin(myVector), end(myVector), x)` where myVector is a std::vector

// TODO: store all NumberSources and SoundSources in shared_ptrs, and use
// shared_ptr or weak_ptr instead of raw pointers wherever it makes sense
// in the core interfaces.
// Would it make sense for all UI objects to be constructible from a shared_ptr
// to the object they interface with?

// TODO: TrackList object (like DAW track list) (with incorporated mixer?)

// TODO: Sequencer (for generating precisely-timed beats and things)
// - cyclical, with custom cycle length
// - multiple sound inputs
// - per-sound and per-input attributes
// - optional timing cues (like 1/4 cycle, 1/8 cycle, inner repetition? i.e. 1/2-length sequence repeated twice)

// TODO: remove empty/unused files

// TODO: actually use flo::Network

// TODO: safegaurd sound processing objects from being deleted
// while they are inside getNextChunk
// NOTE: this will require care. getNextChunk is a virtual function,
// so this should best be done at the most derived level

// TODO: include and dynamically link to ffmpeg for additional audio formats?

// TODO: add graphs back in a safe and clean way (for plotting functions)

// TODO: update wires when objects change size

// TODO: safeguard number sources from deletion while they are being used
// (i.e. do some kind of locking to prevent them from being deleted
// during their use in sound processing)

// TODO: allow default value of NumberInput to be edited via NumberInputPeg
// TODO: add posRelativeTo(const Element* ancestor) to ui and use it
// TODO: cut/copy/paste
// TODO: load/save project files (.flo extension???)
// TODO: Collapsible panels
// TODO: obstacle-avoiding wires
// TODO: hand-written math expressions
// TODO: record sound streams into an audioclip
// TODO: prevent peg labels from receiving mouse input (maybe bring back element disabling in ui?)

// TODO: full keyboard naviation
// 'letter' is used to mean that each applicable object
// is highlighted with a letter from the keyboard, and
// pressing that letter selects the indicated object
// - panel scope:
//     - [something] to create an object
//     - arrow keys or letter to select nearby object
// - object scope:
//     - arrow keys or letter to select peg
//     - shift + arrow keys to move object?
//     - escape to return to panel scope
// - peg scope:
//     - letter to select target peg for adding/removing
//       wire (only applicable pegs are indicated)

// TODO: audioclip
// - one-shot vs loop
// - start time
// - wave visualization and editing
// - number inputs for current time OR current speed
// TODO: bandpass filter
// TODO: IDCT
// TODO: IFFT
// TODO: IDWT
// TODO: how to implement vocoder (i.e. synth filtered by human voice) without breaking scope?
// TODO: custom waveform
// TODO: spline (with points that can be moved by numberinputs)
// TODO: cubic spline
// TODO: bezier spline
// TODO: gaussian radial basis function
// TODO: sample-wise function (this should be called WaveShaper)
// TODO: ADSR (number object), takes the following inputs:
//       - current time (in seconds)
//       - attack length (in seconds)
//       - decay length (in seconds)
//       - sustain level (unitless, in range [0, 1])
//       - sustain time (not length, i.e when in time the 'note' is released, in seconds)
//       - release length (in seconds)
//       output is a single value in the range of [0, 1]. This can be used to control an amplifier,
//       filter, note qualities, etc.

//#include <Flosion/Objects/Melody.hpp>
//#include <Flosion/Objects/DAC.hpp>
//#include <Flosion/Objects/Ensemble.hpp>
//#include <Flosion/Objects/WaveGenerator.hpp>
//#include <Flosion/Objects/Functions.hpp>
//#include <Flosion/Objects/WaveForms.hpp>
//#include <Flosion/Objects/ADSR.hpp>
//#include <Flosion/Objects/Lowpass.hpp>

//#include <Flosion/Util/RNG.hpp>
//#include <Flosion/Objects/LiveMelody.hpp>

#include <Flosion/Util/Base64.hpp>

#include <iostream>

int main() {

    auto& win = ui::Window::create(1000, 700, "Flosion");

    win.setRoot<flui::FlosionUI>();

    ui::run();

    return 0;

    /*
    //auto mel = flo::Melody{};
    auto mel = flo::LiveMelody{};

    const auto sfreq = flo::sampleFrequency;

    //mel.addNote(0,         sfreq,     100.0 / 2.0);
    //mel.addNote(sfreq,     sfreq,     125.0 / 2.0);
    //mel.addNote(sfreq * 2, sfreq * 2, 150.0 / 2.0);
    //mel.addNote(sfreq * 3, sfreq,     200.0 / 2.0);


    auto wavegen = flo::WaveGenerator{};
    auto ens = flo::Ensemble{};
    auto lowpass = flo::Lowpass{};

    ens.input.setSource(&wavegen);
    lowpass.input.setSource(&ens);
    mel.input.setSource(&lowpass);

    auto wave = flo::SawWave{};
    wave.input.setSource(&wavegen.phase);
    wavegen.waveFunction.setSource(&wave);

    wavegen.frequency.setSource(&ens.input.frequencyOut);

    ens.frequencySpread.setDefaultValue(0.005);

    // auto oneminus = flo::OneMinus{};
    // auto mul = flo::Multiply{};
    // oneminus.input.setSource(&mel.input.noteProgress);
    // mul.input1.setSource(&oneminus);
    // mul.input1.setSource(&mel.input.noteProgress);
    // mul.input2.setSource(&mel.input.noteFrequency);
    // ens.frequencyIn.setSource(&mul);

    ens.frequencyIn.setSource(&mel.input.noteFrequency);

    auto adsr = flo::ADSR{};

    adsr.currentTime.setSource(&mel.input.noteTime);
    adsr.totalDuration.setSource(&mel.input.noteLength);
    adsr.attackDuration.setDefaultValue(0.2);
    adsr.decayDuration.setDefaultValue(0.2);
    adsr.sustainLevel.setDefaultValue(0.1);
    adsr.timeOfRelease.setDefaultValue(0.8);

    auto mul = flo::Multiply{};

    mul.input1.setSource(&adsr);
    mul.input2.setDefaultValue(2000.0);

    lowpass.cutoff.setSource(&mul);

    auto dac = flo::DAC{};

    dac.soundResult.getInput().setSource(&mel);

    dac.play();

    std::set<flo::LiveMelodyNote*> notes;

    auto dist = std::uniform_int_distribution{ 1, 10 };
    auto& randEng = util::getRandomEngine();

    for (int i = 0; i < 100; ++i) {
        auto f = 40.0f * static_cast<float>(dist(randEng));
        if (auto n = mel.startNote(f)) {
            notes.insert(n);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{ 100 * (i % 3) * (i % 4) });

        auto prev_notes = notes;
        for (const auto& n : prev_notes) {
            if (dist(randEng) < 4) {
                mel.stopNote(n);
                notes.erase(n);
            }
        }
    }

    dac.stop();
    
    return 0*/

}
