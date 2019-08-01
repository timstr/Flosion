#include <SFML/Graphics.hpp>

#include <Network.hpp>
#include <SoundResult.hpp>
#include <SoundState.hpp>
#include <SoundSourceTemplate.hpp>
#include <MultiSoundInput.hpp>
#include <NumberNode.hpp>

#include <iostream>
#include <random>

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

class Sine : public flo::StatelessNumberSource {
public:
    Sine() : flo::StatelessNumberSource(compute), input(this) {
    
    }

    flo::NumberSourceInput input;

private:
    static double compute(const flo::NumberSource* self, const flo::SoundState* context) noexcept {
        const auto derivedSelf = reinterpret_cast<const Sine*>(self);
        return std::sin(derivedSelf->input.getValue(context) * 2.0 * 3.141592654);
    }
};

class Saw : public flo::StatelessNumberSource {
public:
    Saw() : flo::StatelessNumberSource(compute), input(this) {
    
    }

    flo::NumberSourceInput input;

private:
    static double compute(const flo::NumberSource* self, const flo::SoundState* context) noexcept {
        const auto derivedSelf = reinterpret_cast<const Saw*>(self);
        const auto v = derivedSelf->input.getValue(context);
        return 2.0 * (v - std::floor(v)) - 1.0;
    }
};

class SmootherState : public flo::State {
public:
    void reset() noexcept override {
        value = 0.0;
    }

    double value{};
};
class Smoother : public flo::BorrowingNumberSource<SmootherState> {
public:
    Smoother() : flo::BorrowingNumberSource<SmootherState>(compute), input(this) {
    
    }

    flo::NumberSourceInput input;

private:
    static double compute(const flo::BorrowingNumberSource<SmootherState>* self, SmootherState* state) noexcept {
        auto derivedSelf = reinterpret_cast<const Smoother*>(self);
        auto v = derivedSelf->input.getValue(derivedSelf->getStateLender()->getMainState(state));
        state->value += 1.0 * (v - state->value);
        return state->value;
    }
};

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
    Oscillator() : phase(this), waveFunction(this), frequency(this) {

    }

    void renderNextChunk(flo::SoundChunk& chunk, OscillatorState* state){
        for (size_t i = 0; i < flo::SoundChunk::size; ++i){
            float val = static_cast<float>(waveFunction.getValue(state));
            chunk.l(i) = val;
            chunk.r(i) = val;
            state->phase += frequency.getValue(state);
        }
    }

    class Phase : public flo::SoundNumberSource<Oscillator> {
    public:
        Phase(Oscillator* parent) : SoundNumberSource(parent, getValue) {
            
        }

        static double getValue(const flo::SoundNumberSource<Oscillator>* self, const OscillatorState* state) noexcept {
            return state->phase;
        }
    } phase;

    flo::SoundNumberInput waveFunction;
    flo::SoundNumberInput frequency;
};

class EnsembleInputState : public flo::SoundState {
public:
    using SoundState::SoundState;

    void reset() noexcept override {
        frequency = 0.0;
    }

    double frequency {};
};

class EnsembleState : public flo::SoundState {
public:
    using SoundState::SoundState;

    void reset() noexcept override {
        buffer.silence();
        init = false;
    }

    flo::SoundChunk buffer;
    bool init {};
};

class Ensemble : public flo::Realtime<flo::ControlledSoundSource<EnsembleState>> {
public:
    static const size_t numVoices = 2;
    
    Ensemble() {
        addDependency(&input);
        for (size_t k = 0; k < numVoices; ++k){
            input.addKey(k);
        }
    }
    ~Ensemble(){
        for (size_t k = 0; k < numVoices; ++k){
            input.removeKey(k);
        }
        removeDependency(&input);
    }
    
    void renderNextChunk(flo::SoundChunk& chunk, EnsembleState* state){
        if (!state->init){
            for (size_t k = 0; k < numVoices; ++k){
                input.getState(this, state, k)->frequency = 0.05 + (static_cast<double>(k) * 0.03);
            }
        }
        for (size_t k = 0; k < numVoices; ++k){
            input.getNextChunkFor(state->buffer, this, state, k);
            for (size_t i = 0; i < flo::SoundChunk::size; ++i){
                chunk[i] += state->buffer[i] * 0.1f;
            }
        }
    }

    class Input : public flo::MultiSoundInput<EnsembleInputState, size_t> {
    public:
        Input() : frequency(this) {
        }

        class Frequency : public flo::SoundNumberSource<Input> {    
        public:
            Frequency(Input* owner) : SoundNumberSource<Input>(owner, compute) {
                
            }
        private:
            static double compute(const SoundNumberSource<Input>*, const EnsembleInputState* state) noexcept {
                return state->frequency;
            }
        } frequency;
    } input;
};

int main() {

	// Testing:
	flo::Network network;

    auto osc = Oscillator{};

    auto smoo = Smoother{};
    smoo.borrowFrom(&osc);

    auto sine = Sine{};
    auto saw = Saw{};

    auto ens = Ensemble{};

    sine.input.setSource(&osc.phase);
    saw.input.setSource(&osc.phase);

    std::random_device rd;
    auto eng = std::default_random_engine{rd()};
    auto dist = std::uniform_int_distribution<int>{0,1};
    if (dist(eng)){
        smoo.input.setSource(&sine);
    } else {
        smoo.input.setSource(&saw);
    }

    ens.input.setSource(&osc);

    osc.waveFunction.setSource(&smoo);

    // osc.frequency.setDefaultValue(0.05);
    osc.frequency.setSource(&ens.input.frequency);

    auto res = flo::SoundResult{};

    auto chunk = flo::SoundChunk{};

    res.setSource(&ens);

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