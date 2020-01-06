#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class AccumulatorState : public flo::State {
    public:
        void reset() noexcept override;

        double value{};
    };

    class Accumulator : public flo::BorrowingNumberSourceTemplate<AccumulatorState> {
    public:
        Accumulator();

        flo::NumberSourceInput input;

    private:
        double evaluate(AccumulatorState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
