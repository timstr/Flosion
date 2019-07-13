#include <SFML/Graphics.hpp>

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

class OscillatorState : public flo::SoundState {
public:
    OscillatorState(flo::SoundNode* owner, const flo::SoundState* dependentState)
        : SoundState(owner,dependentState)
        , phase(0.0) {}

    void reset() noexcept override {
        phase = 0.0;
    }

    double phase;
}; 

class Oscillator : public flo::SoundSourceTemplate<OscillatorState> {
public:
    Oscillator(flo::SoundNetwork* network) : SoundNode(network, Type::Normal), SoundSourceTemplate(network, Type::Normal) {}

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
	flo::SoundNetwork network;

    auto osc = Oscillator{&network};



	return 0;
}