#pragma once

#include <Flosion/Objects/FunctionsBase.hpp>

namespace flo {

    class Add : public BinaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Subtract : public BinaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Multiply : public BinaryFunction {
    public:
        Multiply();

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Divide : public BinaryFunction {
    public:
        Divide();

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    // TODO: more functions

} // namespace flo
