#include "musical.h"

#include <SFML/Graphics.hpp>
#include <math.h>
#include "filebrowser.h"
#include <chrono>

#include <GUI/GUI.hpp>

#include "constantsUI.h"
#include "functionsUI.h"
#include "dacUI.h"
#include "audioUI.h"
#include "wavegenUI.h"
#include "timestretchUI.h"
#include "samplerUI.h"
#include "ensembleUI.h"
#include "splineUI.h"
#include "filterUI.h"
#include "phasevocoderUI.h"
#include "convolverUI.h"
#include "distortionUI.h"
#include "wavetableUI.h"

#include <iostream>
#include <iomanip>

#include "fft.h"

// TODO: update to new gui
// TODO: rethink sound processing design
// TODO: add distinction for sound processors that continuously stream their source,
//	since these would make sense for allowing midi/keyboard/other user input
// TODO: General code review
// TODO: make better use of the following:
// - RVO
// - rvalue references
// - correct value/const-ref passing
// - move semantics
// - constexpr
// - noexcept
// - const correctness
// - smart pointers where appropriate
// - use very few pointers in general; use references, store by value where possible
// - std::array
// - templated functions/types with perfect forwarding
// TODO: use assert() macro and noexcept instead of if ... throw
// TODO: don't use rand() anywhere, use C++11 <random>
// TODO: add facility for precomputed functions?
//	- i.e. for fft window, bell curve, to-amplitude function, sin, cos, log, etc
//	- see https://stackoverflow.com/questions/19016099/lookup-table-with-constexpr

// TODO: synchronize this stuff
// - See https://www.youtube.com/watch?v=boPEO2auJj4
// - Consider using std::atomic<T> for numeric members used in sound processing
// - Consider using std::atomic<shared_ptr<T>> (or equivalent) for more complex types
// - States are exempt from this (yay!!!), since they are only modified by the audio thread
// - Any value which can be directly be changed by the gui should use
//   atomics however (like number sliders, constants, any member data of
//   sound processors that is configurable by the GUI and isn't controlled by wires)
// - Changes to wiring *need* to be synchronized as they have caused many crashes
//   to date.

// TODO: cache things
// TODO: scatter
// TODO: ring mod
// TODO: feedback
// TODO: make window sizes customizable for filter
// TODO: clipboard
// TODO: serialization and file loading/saving

// TODO: include and dynamically link to ffmpeg for additional audio formats

int main() {

	ui::init(1000, 700, "Flosion", 30);

	ui::root().add<fui::FlosionUI>();

	ui::run();

	return 0;
}