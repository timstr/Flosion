#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flo {

    class OscillatorState : public flo::State {
    public:
        void reset() noexcept override;

        double value = {};
        double prevReset = {};
    };


    /**
     * Oscillator accumulates a stored value by adding its input each time it
     * is called upon, much like Accumulator, but the output value is wrapped
     * to the unit half-open interval [0, 1)
     */
    class Oscillator : public flo::BorrowingNumberSourceTemplate<OscillatorState> {
    public:
        Oscillator();

        /**
         * The amount that is added to the stored value each time the Oscillator
         * is evaluated.
         * Note that when this is called for each sample in an audio stream, you
         * will typically want to divide the input by the sampling frequency
         */
        flo::NumberSourceInput input;

        /**
         * When this input increases relative to the last evaluation, the stored
         * value is reset to zero.
         */
        flo::NumberSourceInput reset;

    private:
        double evaluate(OscillatorState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
