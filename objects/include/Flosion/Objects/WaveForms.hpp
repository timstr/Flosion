#pragma once

#include <Flosion/Objects/FunctionsBase.hpp>

namespace flo {
    
    class SineWave : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class SawWave : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class SquareWave : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class TriangleWave : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class PulseWave : public NumberSource {
    public:
        PulseWave();

        NumberSourceInput input;
        NumberSourceInput width;

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
