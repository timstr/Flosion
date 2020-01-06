#include <Flosion/Objects/ExponentialSmoother.hpp>

namespace flo {

    void ExponentialSmootherState::reset() noexcept {
        value = 0.0;
    }

    ExponentialSmoother::ExponentialSmoother()
        : input(this)
        , decayRate(this, 0.1) {

    }

    double ExponentialSmoother::evaluate(ExponentialSmootherState* state, const flo::SoundState* context) const noexcept {
        auto v = input.getValue(context);
        auto d = decayRate.getValue(context);
        state->value += d * (v - state->value);
        return state->value;
    }

} // namespace flo
