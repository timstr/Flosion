#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class VariableState : public flo::State {
    public:
        void reset() noexcept override;

        double value{};
    };

    class Variable : public flo::BorrowingNumberSourceTemplate<VariableState> {
    public:
        Variable();

        flo::NumberSourceInput input;

    private:
        double evaluate(VariableState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
