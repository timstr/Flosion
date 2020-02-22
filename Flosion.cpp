#include <Flosion/UI/Core/FlosionUI.hpp>

// TODO: review GUI things where elements are moved between containers
// (such when borrowers are attached to soundsources) and make use of
// new UI guarantees about event listeners remaining attached

// TODO: remove "std::" where it is not needed thanks to ADL, such as in
// `find(begin(myVector), end(myVector), x)` where myVector is a std::vector

// TODO: remove empty/unused files

// TODO: actually use flo::Network

// TODO: safegaurd sound processing objects from being deleted
// while they are inside getNextChunk

// TODO: include and dynamically link to ffmpeg for additional audio formats?

// TODO: add graphs back in a safe and clean way (for plotting functions)

// TODO: update wires when objects change size

// TODO: safeguard number sources from deletion while they are being used
// (i.e. do some kind of locking to prevent them from being deleted
// during their use in sound processing)

// TODO: allow default value of NumberInput to be edited via NumberInputPeg
// TODO: add posRelativeTo(const Element* ancestor) to ui and use it
// TODO: serialization/deserialization for objects and wires
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
// TODO: sample-wise function
// TODO: ADSR (number object), takes the following inputs:
//       - attack time (in seconds)
//       - decay time (in seconds)
//       - sustain level (unitless, in range [0, 1])
//       - total elapsed time until end of release (i.e when in time the 'note' is released, in seconds)
//       - release time
//       output is a single value in the range of [0, 1]. This can be used to control an amplifier,
//       filter, note qualities, etc.

#include <Flosion/Objects/Melody.hpp>
#include <Flosion/Objects/DAC.hpp>
#include <Flosion/Objects/Ensemble.hpp>
#include <Flosion/Objects/WaveGenerator.hpp>
#include <Flosion/Objects/Functions.hpp>
#include <Flosion/Objects/WaveForms.hpp>

int main() {

    auto mel = flo::Melody{};
    
    const auto sfreq = flo::sampleFrequency;
    mel.addNote(0,         sfreq,     100.0);
    mel.addNote(sfreq,     sfreq,     125.0);
    mel.addNote(sfreq * 2, sfreq * 2, 150.0);
    mel.addNote(sfreq * 3, sfreq,     200.0);

    auto wavegen = flo::WaveGenerator{};
    auto ens = flo::Ensemble{};
    
    ens.input.setSource(&wavegen);
    mel.input.setSource(&ens);

    auto wave = flo::SquareWave{};
    wave.input.setSource(&wavegen.phase);
    wavegen.waveFunction.setSource(&wave);

    wavegen.frequency.setSource(&ens.input.frequencyOut);

    ens.frequencySpread.setDefaultValue(0.01);

    //auto oneminus = flo::OneMinus{};
    auto mul = flo::Multiply{};
    //oneminus.input.setSource(&mel.input.noteProgress);
    //mul.input1.setSource(&oneminus);
    mul.input1.setSource(&mel.input.noteProgress);
    mul.input2.setSource(&mel.input.noteFrequency);
    ens.frequencyIn.setSource(&mul);

    //wavegen.frequency.setSource(&mel.input.noteFrequency);

    auto dac = flo::DAC{};

    dac.soundResult.getInput().setSource(&mel);

    dac.play();
    
    std::this_thread::sleep_for(std::chrono::seconds{9});
    mel.setLength(2 * flo::sampleFrequency);
    std::this_thread::sleep_for(std::chrono::seconds{8});

    dac.stop();

    /*auto& win = ui::Window::create(1000, 700, "Flosion");

    win.setRoot<flui::FlosionUI>();

    ui::run();*/

    return 0;
}