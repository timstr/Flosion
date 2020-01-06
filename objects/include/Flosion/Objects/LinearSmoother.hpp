#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class LinearSmootherState : public flo::State {
    public:
        void reset() noexcept override;

        double value{};
    };

    class LinearSmoother : public flo::BorrowingNumberSourceTemplate<LinearSmootherState> {
    public:
        LinearSmoother();

        flo::NumberSourceInput input;
        flo::NumberSourceInput rate;

    private:
        double evaluate(LinearSmootherState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
