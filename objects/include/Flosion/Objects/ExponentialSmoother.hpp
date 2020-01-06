#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class ExponentialSmootherState : public flo::State {
    public:
        void reset() noexcept override;

        double value{};
    };

    class ExponentialSmoother : public flo::BorrowingNumberSourceTemplate<ExponentialSmootherState> {
    public:
        ExponentialSmoother();

        flo::NumberSourceInput input;
        flo::NumberSourceInput decayRate;

    private:
        double evaluate(ExponentialSmootherState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
