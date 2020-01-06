#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class FlipFlopState : public flo::State {
    public:
        void reset() noexcept override;

        double storedValue = {};
        double prevClock = {};
    };

    class FlipFlop : public flo::BorrowingNumberSourceTemplate<FlipFlopState> {
    public:
        FlipFlop();

        flo::NumberSourceInput input;
        flo::NumberSourceInput clock;

    private:
        double evaluate(FlipFlopState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
