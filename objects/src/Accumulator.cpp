#include <Flosion/Objects/Accumulator.hpp>

namespace flo {

    void AccumulatorState::reset() noexcept {
        value = 0.0;
    }



    Accumulator::Accumulator()
        : input(this) {

    }

    double Accumulator::evaluate(AccumulatorState* state, const flo::SoundState* context) const noexcept {
        return state->value += input.getValue(context);
    }

} // namespace flo
