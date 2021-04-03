#include <Flosion/UI/Core/FlosionUI.hpp>

// TODO: add a new category of sound processors that measure and provide numeric information
// from a parallel stream, but do not change their own stream's sound and simply forward it
// Example: Synth vocoder - one stream is a human voice, another stream is a neat synth sound.
// The human voice stream is passed into an FFT object which measures the frequency spectrum
// and exposes this via number in/outputs. The synth is passed into a filter object, which
// queries the amplitudes of the FFT object *even though it is in a separate stream* and uses
// these to modulate the sound of the synth. The filtered synth is then played and a vocoder
// effect is heard.
// Considerations:
// - Do the voice and synth streams need to be connected to a common downstream object (e.g.
//   a mixer, with the voice silenced)?
//   -> This seems dumb, since the voice will never be directly used in this example, but
//      this does make more sense because the voice and synth streams would explicitly be
//      part of the same sound processing graph.
// - The voice and synth streams need to be synchronized and execute in lockstep. For every
//   chunk of audio, the voice stream needs to be rendered first because the synth stream
//   depends on it.
//   -> It *should* be possible to reuse the voice stream for other, unrelated downstream
// - Would an explicit 'stream merge' make sense here? This could be a simple and comprehensible
//   way to achieve these kinds of unintrusive synchronized measurement operations in a safe way.
//   How this could look:
//    - 
// #error 
//    - ???

// Priorities:
// TODO: turn on conformance mode in MSVC (add "/permisive-" to compiler flags)
// TODO: actually use flo::Network
// Option 1: every object is created and owned by Network as in Network::create<WaveGenerator>(...)
// and is used via reference
// - similar to tims-gui interface
// - Ownership of objects by network is clear and simple
// - UI objects will no longer own objects, but should be constructible from a reference to an existing object
//   - !!! This alone may fix many initialization order problems (i.e. references to sound object being handed out by
//     base UI class before derived UI class constructs sound object member)
//     - This is a huge advantage
//   - This may require significant refactoring
//     - Constructor may make no assumptions about state of object, and should fully initialize
//       itself according to the state of the object it is constructed from
//       - This sounds like a good thing, although it will easily lead to duplicated code (e.g. code in both constructor
//         and event-listeners must do same thing)
//       - It would be really nice to have a ReactJS-style UI driving this (e.g. UI object is constructed from observable
//         properties and updates itself automatically when those change)
//         - That's a whole other can of worms but darn, it really would be nice.
//     - Serialization will need to be done differently
//       - sound/number objects should probably be serialized first, then their UI objects
//     - UI Objects might need to be selected at runtime based on dynamic type of sound/number object
//       - Existing serialization mapping could be adapted for this
// Option 2: every object must explicitly be added to a Network
// - I don't like this
// TODO: find a safe way and clean to delay the initial state allocation
//       by Uncontrolled (monostate) objects. State should not be allocated
//       until the most-derived class has been constructed, since states may
//       refer to this class (i.e. MelodyState, when querying note overlap).
//       Two-phase initialization (see isocpp.org) could be used in conjunction
//       with some sort of Network::create<ObjectType>(args...) factory method 
//       which calls an initPhaseTwo() method or similar
// TODO: find a safe and clean way to ensure that sound/number sources
//       contained by UI objects are constructed before base classes are given
//       pointers to these from a derived class that is not yet fully constructed.
//       --> Can this also be solved with the above problem?
// TODO: it should not be possible to pass an Uncontrolled sound source to an OutOfSync sound input

// ReactJS-style UI thoughts
// ---------------------------------------------------
// return VerticalList(
//     Text(m_audioClip.fileName).underline(),
//     Button()
//         .caption("Load")
//         .onClick([&](){ auto path = showOpenFileDialog(); m_audioClip.loadFrom(path);}),
//     Button()
//         .caption(m_audioClip.loop.get([](bool v){ return v ? "Loop" : "One-shot"; }) // NOTE: get(fn) returns some object that sends derived update signals whenever m_audioClip.loop is updated, similar to Reselect library in JavaScript
//         .onClick([&](){ m_audioClip.loop.set(!m_audioClip.loop.getOnce(); })
//     );
// ---------------------------------------------------

// TODO: try to eliminate uses of virtual methods and replace them with template
// based dispatching, for example in:
// * SoundSource::getNextChunkFor, which defers to:
//   * UncontrolledSoundSource::renderNextChunk
//   * ControlledSoundSource::renderNextChunk
// * NumberSource::evaluate, which defers to:
//   * BorrowingNumberSourceTemplate::evaluate
//   * SoundNumberSource::evaluate
// An alternative approach:
//   This additional virtual function call can probably be replaced with some kind of CRTP method like
//   struct SoundSource {
//       virtual void getNextChunkFor(...) = 0; // needed
//   };
//   
//   template<typename Derived, typename StateTypeWhatever>
//   struct SoundSourceImpl {
//       virtual void getNextChunkFor(...) override final {
//           // do stuff, maybe some compile-time logic based on template parameters
//           static_cast<Derived*>(this)->renderNextChunk(...); // Derived must supply this method (static_assert could help)
//           // finish up
//       }
//   };
//
//   struct MySoundSource : SoundSourceImpl<MySoundSource, MyStateType> {
//       void renderNextChunk(...) {
//           // do something, anything at all
//       }
//   };


// TODO: use std::byte instead of unsigned char in StateTable

// TODO: use propert library tools in StateTable implementation like:
// - std::launder
// - std::construct_at
// - std::destroy_at
// - std::align (probably not useful, can't be used to determine memory needs ahead of time)
// See useful example code at https://en.cppreference.com/w/cpp/types/aligned_storage

// TODO: once multithreaded audio processing is figured out, consider using cache line size to determine
// layout of StateTable using
// - std::hardware_destructive_interference_size
// - std::hardware_constructive_interference_size


// TODO: onChange signal for global number sources?

// TODO: make clear visual distinction between SoundObjects, NumberObjects, and BorrowingNumberObjects

// TODO: clearly attach multisoundinput-specific number inputs/outputs to areas representing those inputs
// i.e. NoteFrequency, NoteProgress should not go on Sampler's main body, but rather on a smaller
// container for the multisoundinput

// TODO: oscilloscope

// TODO: D O C U M E N T A T I O N
// Use Doxygen to better keep documentation consistent with the code itself
// See http://www.doxygen.nl/

// TODO: smoothing options for Resampler (nearest / linear / cubic)

// TODO: PulseWave (square wave with adjustable pulse width, average amplitude always remains zero)

// TODO: make objects created via PanelContextMenu appear
// centered where PanelContextMenu was made (i.e where mouse was double-clicked)

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

// - UI for live melody
// - UI for melody?

// TODO: hot swapping

// TODO: review GUI things where elements are moved between containers
// (such as when borrowers are attached to soundsources) and make use of
// new UI guarantees about event listeners remaining attached

// TODO: remove "std::" where it is not needed thanks to ADL, such as in
// `find(begin(myVector), end(myVector), x)` where myVector is a std::vector

// TODO: TrackList object (like DAW track list) (with incorporated mixer?)

// TODO: Sequencer (for generating precisely-timed beats and things)
// - cyclical, with custom cycle length
// - multiple sound inputs
// - per-sound and per-input attributes
// - optional timing cues (like 1/4 cycle, 1/8 cycle, inner repetition? i.e. 1/2-length sequence repeated twice)

// TODO: remove empty/unused files

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
// TODO: hand-written math expressions (i.e. typing "exp(-x^2)/4y" creates a (panel containing a) group of connected number nodes representing the expression)
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
// - start time
// - wave visualization and editing
// - number inputs for current time OR current speed
// TODO: bandpass filter
// TODO: IDCT
// TODO: IFFT
// TODO: IDWT
// TODO: how to implement vocoder (i.e. synth filtered by human voice) without breaking scope?
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
