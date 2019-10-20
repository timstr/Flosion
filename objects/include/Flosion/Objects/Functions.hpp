#pragma once

#include <Flosion/Core/NumberSource.hpp>

namespace flo {
    
    class Add : public flo::NumberSource {
    public:
        Add();

        flo::NumberSourceInput inputA;
        flo::NumberSourceInput inputB;

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Multiply : public flo::NumberSource {
    public:
        Multiply();

        flo::NumberSourceInput inputA;
        flo::NumberSourceInput inputB;

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
