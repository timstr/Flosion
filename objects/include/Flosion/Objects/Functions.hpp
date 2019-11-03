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

    /*
    Unary:
        abs
        sqrt
        cbrt
        sqr
        log
        log2
        log10
        exp
        exp2
        exp10
        sin
        cos
        tan
        asin
        acos
        atan
        sinh
        cosh
        tanh
        asinh
        acosh
        atanh
        ceil
        floor
        round
        plusOne
        minusOne
        oneMinus
        negate
        inverse
        stdToNorm (maps [-1,1] to [0,1])
        normToStd (maps [0,1] to [-1,1]
        sigmoid

    Binary
        min
        max
        pow
        logb
        hypot
        atan2

    Other
        gaussian
        summation

    */

} // namespace flo
