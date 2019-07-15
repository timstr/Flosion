#include <SFML/Graphics.hpp>

#include <Network.hpp>
#include <SoundResult.hpp>
#include <SoundState.hpp>
#include <SoundSourceTemplate.hpp>

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

// TODO: include and dynamically link to ffmpeg for additional audio formats?

// TODO: add graphs back (in a safe and clean way)

class Oscillator;

class OscillatorState : public flo::ConcreteSoundState<Oscillator> {
public:
    using ConcreteSoundState::ConcreteSoundState;

    void reset() noexcept override {
        phase = 0.0;
    }

    double phase {};
}; 

class Oscillator : public flo::SoundSourceTemplate<OscillatorState> {
public:
    Oscillator() : SoundSourceTemplate(Controllability::Controllable, TimeSync::Realtime) {}

    void renderNextChunk(flo::SoundChunk& chunk, OscillatorState* state){
        for (size_t i = 0; i < flo::SoundChunk::size; ++i){
            float val = std::sin(static_cast<float>(state->phase));
            chunk.l(i) = val;
            chunk.r(i) = val;
            state->phase += 0.1;
        }
    }
};



int main() {

	// Testing:
	flo::Network network;

    auto osc = Oscillator{};

    auto res = flo::SoundResult{};

    auto chunk = flo::SoundChunk{};

    res.setSource(&osc);

    res.getNextChunk(chunk);

	return 0;
}