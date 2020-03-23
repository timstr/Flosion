#include <Flosion/Objects/Accumulator.hpp>

namespace flo {

    void AccumulatorState::reset() noexcept {
        value = 0.0;
        prevReset = std::numeric_limits<double>::min();
    }



    Accumulator::Accumulator()
        : input(this)
        , reset(this) {

    }

    double Accumulator::evaluate(AccumulatorState* state, const flo::SoundState* context) const noexcept {
        state->value += input.getValue(context);
        auto r = reset.getValue(context);
        if (r > state->prevReset) {
            state->value = 0.0;
        }
        state->prevReset = r;
        return state->value;
    }

} // namespace flo
