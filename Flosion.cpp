#include <Flosion/UI/Core/FlosionUI.hpp>

// TODO: clear DAC buffer when resetting

// TODO: remove empty/unused files

// TODO: actually use flo::Network

// TODO: safegaurd sound processing objects from being deleted
// while they are inside getNextChunk

// TODO: include and dynamically link to ffmpeg for additional audio formats?

// TODO: add graphs back (in a safe and clean way)

// TODO: update wires when objects change size

// TODO: safeguard number sources from deletion while they are being used
// (i.e. do some kind of locking to prevent them from being deleted
// during their use in sound processing)

// TODO: allow default value of NumberInput to be edited via NumberInputPeg
// TODO: add posRelativeTo(const Element* ancestor) to ui and use it
// TODO: when a connected head is moused over, show the peg's label
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

// TODO: How to create DCT/FFT/DWT etc which is only intended
// to produce numeric output but needs to be connected as a
// sound source in order to make sense?
// Idea: some object which splices a second audio stream into a main
// audio stream, calling on it in lockstep with the main stream and
// discarding its output, then making its number sources available
// to the main audio stream. (Note: this would enable the vocoder effect
// without breaking scoping rules).
//
//                        ### SPLICER #####                         
//  Side sound stream     #               #                         
// -------------------->(SI)-.   .-[X]-.  #                         
//                        #   \ /       \ #       Main sound stream 
// -------------------->(MI)--(R)--------(MO)-------------------->  
//  Main sound stream     #               #                         
//                        #################                         
//
// Legend:
// MO : Main output (SoundSource)
// MI : Main input (SingleSoundInput)
// SI : Side input (SingleSoundInput)
// R  : Routing node (custom SoundNode)
// X  : Some sound object (probably restricted to Singular Realtime Controlled SoundObjects)
//
// Workflow:
// 1 MO is called upon for the next chunk of audio
// 2 MO sets an internal flag to note that it's using the side stream
// 3 MO calls upon X for the next chunk of audio
// 3.1 When X calls upon R for input, R checks the internal flag and
//     then forwards the call to SI.
// 4 MO discards the chunk produced by X
// 5 MO sets the internal flag to note that it's using the main stream
// 6 MO calls upon R for the next chunk of input
// 6.1 R checks the internal flag and forwards the call to MI

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


int main() {

    auto& win = ui::Window::create(1000, 700, "Flosion");

    win.setRoot<flui::FlosionUI>();

    ui::run();

    return 0;
}