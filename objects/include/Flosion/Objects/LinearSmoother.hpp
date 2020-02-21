#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class LinearSmootherState : public State {
    public:
        void reset() noexcept override;

        double value{};
    };

    class LinearSmoother : public BorrowingNumberSourceTemplate<LinearSmootherState> {
    public:
        LinearSmoother();

        NumberSourceInput input;
        NumberSourceInput rate;

    private:
        double evaluate(LinearSmootherState* state, const SoundState* context) const noexcept override;
    };

} // namespace flo
