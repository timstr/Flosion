#include <SFML/Graphics.hpp>

#include <Network.hpp>
#include <SoundResult.hpp>
#include <SoundState.hpp>
#include <SoundSourceTemplate.hpp>
#include <NumberNode.hpp>

#include <iostream>

#include <conio.h>

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

class Oscillator : public flo::Realtime<flo::ControlledSoundSource<OscillatorState>> {
public:
    /*Oscillator() : phase(this) {

    }*/

    void renderNextChunk(flo::SoundChunk& chunk, OscillatorState* state){
        for (size_t i = 0; i < flo::SoundChunk::size; ++i){
            float val = std::sin(static_cast<float>(state->phase));
            chunk.l(i) = val;
            chunk.r(i) = val;
            state->phase += 0.1;
        }
    }

    class Phase : public flo::SoundNumberSource<Oscillator> {
    public:
        Phase(Oscillator* parent) : SoundNumberSource(parent, getValue) {
            
        }

        static double getValue(const flo::SoundNumberSource<Oscillator>* self, OscillatorState* state) noexcept {
            return state->phase;
        }
    } /* phase */;
};


int main() {

	// Testing:
	flo::Network network;

    auto osc = Oscillator{};

    auto res = flo::SoundResult{};

    auto chunk = flo::SoundChunk{};

    res.setSource(&osc);

    while (true){
        res.getNextChunk(chunk);

        for (size_t i = 0, iEnd = chunk.size; i < iEnd; ++i){
            float v = std::clamp(chunk[i].l(), -1.0f, 1.0f) * 0.5f + 0.5f;
            for (size_t j = 0, jEnd = static_cast<size_t>(v * 64.0f); j != jEnd; ++j){
                std::cout << '#';
            }
            std::cout << '\n';
            _sleep(100);
        }
    }

    _getch();

	return 0;
}