#include <Flosion/Objects/FlipFlop.hpp>

namespace flo {

    void FlipFlopState::reset() noexcept {
        storedValue = 0.0;
        prevClock = 0.0;
    }

    FlipFlop::FlipFlop()
        : input(this)
        , clock(this) {

    }

    double FlipFlop::evaluate(FlipFlopState* state, const flo::SoundState* context) const noexcept {
        auto c = clock.getValue(context);
        if (c > state->prevClock){
            state->storedValue = input.getValue(context);
        }
        state->prevClock = c;
        return state->storedValue;
    }

} // namespace flo
