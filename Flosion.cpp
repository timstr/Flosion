#include <Flosion/UI/Core/FlosionUI.hpp>

// TODO: remove empty/unused files

// TODO: add bound property to sound streams
// - Given a SoundNode, the sound stream it produces, regardless of which state
//   is used, shall be one of the following:
//       - Bounded, with predeterminable length (which may vary by state)
//       - Unbounded or of nonpredeterminable length
// - A sound stream of bounded length shall produce audio until its length
//   is reached, after which it produces silence (in the implementation, it may be
//   possible to automate this).
// - Some sound sources inherently produce a bounded stream, such as an AudioClip.
// - Some sound sources inherently produce an unbounded stream, such as a WaveGenerator.
// - Other sound sources may produce either a bounded or unbounded stream depending
//   on their SoundInputs, such as a LowPass object. (NOTE: objects which change the
//   speed of time should in the initial implementation always produce unbounded streams,
//   but this restriction may be lifted in the future such that time-speed-changing
//   objects may produce bounded streams of adjusted length if their inputs are bounded
//   AND their speed of time is constant throughout the duration of the stream. This
//   will require adding functionality to NumberSources and will require special care,
//   and probably a way to determine constant-ness with respect to specific inputs)
// - SoundInputs always propagate the boundedness of their sources. Specifically,
//   a SoundInput drawing upon a bounded SoundSource is bounded, and a SoundInput drawing
//   upon an unbounded SoundSource is unbounded.
// - A SoundInput without a connected SoundSource produces a bounded stream with zero length.
// - Examples of objects which might adjust the bounds of their inputs:
//     - Mixer: output bound is the maximum of its input bounds
//     - Convolver: output bound is the input bound plus the length of the convolving signal
//     - Delay: output bound is the input bound pluss the delay as determined by a
//       NumberInput. This can be determined by querying the input during initialization
//       (see below) and requiring it to be constant (w.r.t. what?)
//     - AudioClip: unbounded if repeating is turned on, bounded otherwise
//     - Resampler: unbounded if the input is unbounded or the timespeed NumberInput
//       is not constant. Otherwise, the output bound is the input bound divided by the
//       (constant) time speed. NOTE: it obviously doesn't make sense for the speed to
//       be zero if a destination SoundInput is relying upon the Resampler to produce
//       bounded input. This is a special case that should be carefully (and elegantly)
//       prevented, keeping in mind that in more general contexts, it is desirable to
//       sometimes bring the timespeed to zero.
//     - PhaseVocoder: same rules as Resampler
//     - LowPass/HighPass/BandPass/BandReject/Filter: output bound is equal to input bound
// Benefits of boundedness:
//     - A soundstream may be recorded until it is over
//     - Objects that replay their input many times can be made much smarter. For example, the
//       Melody (midi sequence) object can know exactly how long each note will continue to play
//       and can allocate an optimal number of states.
//     - Objects that want to use or display the length of their inputs can do so. For example, the
//       Sequencer object (like a DAW track list) can automatically determine the lengths
//       of bounded inputs.
//     - ???
// Drawbacks and complications of boundedness:
//     - The SoundNode interface will get more complex, but it should be possible to hide most
//       of this with mixin templates similar to what is in use for Divergent/Singular,
//       Realtime/OutOfSync, and Controlled/Uncontrolled. In fact, Bounded/Unbounded/MaybeBounded
//       seems to fit right in.
//     - It's unclear how much these properties may be relied on, and what should happen when
//       they change. For example:
//           - What should happen when a SoundInput which has been drawing upon a bounded stream
//             is reconnected, while it is running, to an unbounded stream? In some cases, this
//             will be harmless; in other cases, this should perhaps be prevented from happening.
//     - SoundSources that need to know the length of their inputs will probably need to know
//       before the stream begins. Perhaps this could be determined when inputs are connected
//       by querying the connected SoundInput for its boundedness and length *prior to* to the
//       first call to getNextChunk. This may prove useful elsewhere, and shall henceforth be
//       referred to as "initialization"
//     - ???

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