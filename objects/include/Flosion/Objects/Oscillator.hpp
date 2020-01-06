#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class OscillatorState : public flo::State {
    public:
        void reset() noexcept override;

        double value{};
    };

    class Oscillator : public flo::BorrowingNumberSourceTemplate<OscillatorState> {
    public:
        Oscillator();

        flo::NumberSourceInput input;

    private:
        double evaluate(OscillatorState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
