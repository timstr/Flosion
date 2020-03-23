#include <Flosion/Objects/Oscillator.hpp>

namespace flo {

    void OscillatorState::reset() noexcept {
        value = 0.0;
        prevReset = std::numeric_limits<double>::min();
    }



    Oscillator::Oscillator()
        : input(this)
        , reset(this) {

    }

    double Oscillator::evaluate(OscillatorState* state, const flo::SoundState* context) const noexcept {
        state->value += input.getValue(context);
        state->value -= std::floor(state->value);
        auto r = reset.getValue(context);
        if (r > state->prevReset) {
            state->value = 0.0;
        }
        state->prevReset = r;
        return state->value;
    }

} // namespace flo
