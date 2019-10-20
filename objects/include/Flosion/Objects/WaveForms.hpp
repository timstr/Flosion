#pragma once

#include <Flosion/Objects/FunctionsBase.hpp>

namespace flo {
    
    class Sine : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Saw : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
