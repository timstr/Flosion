#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class ExponentialSmootherState : public flo::State {
    public:
        void reset() noexcept override;

        double value = {};
        double prevReset = {};
    };

    class ExponentialSmoother : public flo::BorrowingNumberSourceTemplate<ExponentialSmootherState> {
    public:
        ExponentialSmoother();

        NumberSourceInput input;
        NumberSourceInput decayRate;
        NumberSourceInput reset;

    private:
        double evaluate(ExponentialSmootherState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
