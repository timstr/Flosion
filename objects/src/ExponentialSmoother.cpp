#include <Flosion/Objects/ExponentialSmoother.hpp>

namespace flo {

    void ExponentialSmootherState::reset() noexcept {
        value = 0.0;
        prevReset = std::numeric_limits<double>::min();
    }

    ExponentialSmoother::ExponentialSmoother()
        : input(this)
        , decayRate(this, 0.1)
        , reset(this) {

    }

    double ExponentialSmoother::evaluate(ExponentialSmootherState* state, const flo::SoundState* context) const noexcept {
        auto v = input.getValue(context);
        auto d = decayRate.getValue(context);
        state->value += d * (v - state->value);
        auto r = reset.getValue(context);
        if (r > state->prevReset) {
            state->value = v;
        }
        return state->value;
    }

} // namespace flo
