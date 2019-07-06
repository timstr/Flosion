#include <SFML/Graphics.hpp>

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

// TODO: compiler optimizations:
// - consider /fp:fast
// - consider /Oi
// - consider /arch:SSE /arch:SSE2 /arch:AVX /arch:AVX2
// - https://docs.microsoft.com/en-us/cpp/build/reference/profile-guided-optimizations?view=vs-2017

// TODO: add graphs back (in a safe and clean way)

// TODO: allow text comments to be added free-floating or to a processing object

int main() {

	// Testing:
	/*using namespace std::chrono_literals;

	flo::Audio audio;
	flo::PhaseVocoder pv;
	flo::Constant speed;
	flo::DAC dac;

	pv.input.setSource(&audio);
	pv.timespeed.setSource(&speed);
	dac.input.input.setSource(&pv);
	speed.setValue(0.1f);
	audio.loadFromFile("C:/Users/Tim/Music/sounds/1981.ogg");
	std::this_thread::sleep_for(3s);

	dac.play();
	std::this_thread::sleep_for(5s);
	speed.setValue(0.01f);
	std::this_thread::sleep_for(30s);
	dac.pause();
	std::this_thread::sleep_for(1s);

	dac.input.input.setSource(nullptr);*/

    /*
	ui::init(1000, 700, "Flosion", 30);
	ui::root().add<fui::FlosionUI>();
	ui::run();
    */

	return 0;
}