#pragma once

#include <Flosion/Objects/FunctionsBase.hpp>

#include <cmath>
#include <random>

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

    class PiConstant : public flo::NumberSource {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class EulersConstant : public flo::NumberSource {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class TauConstant : public flo::NumberSource {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class SampleFrequencyConstant : public flo::NumberSource {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Abs : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class SquareRoot : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class CubeRoot : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Square : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Log : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Log2 : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Log10 : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Exp : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Exp2 : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Exp10 : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Sin : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Cos : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Tan : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Asin : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Acos : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Atan : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Sinh : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Cosh : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Tanh : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Asinh : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Acosh : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Atanh : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Ceil : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Floor : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Round : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Frac : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class PlusOne : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class MinusOne : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class OneMinus : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };
    
    class Negate : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Reciprocal : public UnaryFunction {
    public:
        Reciprocal();

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    // maps [-1,1] to [0,1] linearly
    class StdToNorm : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    // maps [0,1] to [-1,1] linearly
    class NormToStd : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Sigmoid : public UnaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Min : public BinaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Max : public BinaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Pow : public BinaryFunction {
    public:
        Pow();
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class LogBase : public BinaryFunction {
    public:
        LogBase();
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Hypot : public BinaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Atan2 : public BinaryFunction {
    public:
        Atan2();
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class RandomUniform : public BinaryFunction {
    public:
        RandomUniform();

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;

        mutable std::uniform_real_distribution<double> m_dist;
    };

    class RandomNormal : public BinaryFunction {
    public:
        RandomNormal();

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;

        mutable std::normal_distribution<double> m_dist;
    };

    class RoundTo : public BinaryFunction {
    public:
        RoundTo();

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class FloorTo : public BinaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class CeilTo : public BinaryFunction {
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Remainder : public BinaryFunction {
    public:
        Remainder();

    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class Gaussian : public flo::NumberSource {
    public:
        Gaussian();

        NumberSourceInput input;
        NumberSourceInput center;
        NumberSourceInput width;
        NumberSourceInput amplitude;
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

    class LinearInterpolation : public flo::NumberSource {
    public:
        LinearInterpolation();

        NumberSourceInput start;
        NumberSourceInput end;
        NumberSourceInput fraction;
    private:
        double evaluate(const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
