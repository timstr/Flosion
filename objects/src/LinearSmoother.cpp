#include <Flosion/Objects/LinearSmoother.hpp>

namespace flo {

    void LinearSmootherState::reset() noexcept {
        value = 0.0;
    }

    LinearSmoother::LinearSmoother()
        : input(this)
        , rate(this) {
    }

    double LinearSmoother::evaluate(LinearSmootherState* state, const flo::SoundState* context) const noexcept {
        const auto i = input.getValue(context);
        const auto r = std::abs(rate.getValue(context));
        const auto d = i - state->value;
        if (std::abs(d) < r){
            state->value = i;
        } else {
            state->value += (d > 0.0 ? r : -r);
        }
        return state->value;
    }

} // namespace flo
