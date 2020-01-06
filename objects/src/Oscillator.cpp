#include <Flosion/Objects/Oscillator.hpp>

namespace flo {

    void OscillatorState::reset() noexcept {
        value = 0.0;
    }



    Oscillator::Oscillator()
        : input(this) {

    }

    double Oscillator::evaluate(OscillatorState* state, const flo::SoundState* context) const noexcept {
        state->value += input.getValue(context);
        state->value -= std::floor(state->value);
        return state->value;
    }

} // namespace flo
