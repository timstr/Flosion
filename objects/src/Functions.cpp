#include <Flosion/Objects/Functions.hpp>

#include <Flosion/Util/RNG.hpp>

namespace flo {
    
    namespace {
        const double pi = 3.141592653589793;
        const double tau = 6.283185307179586;
        const double eulersConstant = 2.718281828459045;
    }

    double Add::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) + input2.getValue(context);
    }

    Multiply::Multiply() : BinaryFunction(1.0, 1.0) {
        
    }

    double Multiply::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) * input2.getValue(context);
    }

    double Subtract::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) - input2.getValue(context);
    }

    Divide::Divide() : BinaryFunction(1.0, 1.0) {

    }

    double Divide::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) / input2.getValue(context);
    }

    double PiConstant::evaluate(const flo::SoundState * context) const noexcept {
        return pi;
    }

    double EulersConstant::evaluate(const flo::SoundState*) const noexcept {
        return eulersConstant;
    }

    double TauConstant::evaluate(const flo::SoundState * context) const noexcept {
        return tau;
    }

    double Abs::evaluate(const flo::SoundState * context) const noexcept {
        return std::abs(input.getValue(context));
    }

    double SquareRoot::evaluate(const flo::SoundState * context) const noexcept {
        return std::sqrt(input.getValue(context));
    }

    double CubeRoot::evaluate(const flo::SoundState * context) const noexcept {
        return std::cbrt(input.getValue(context));
    }

    double Square::evaluate(const flo::SoundState * context) const noexcept {
        const auto x = input.getValue(context);
        return x * x * x;
    }

    double Log::evaluate(const flo::SoundState * context) const noexcept {
        return std::log(input.getValue(context));
    }

    double Log2::evaluate(const flo::SoundState * context) const noexcept {
        return std::log2(input.getValue(context));
    }

    double Log10::evaluate(const flo::SoundState * context) const noexcept {
        return std::log10(input.getValue(context));
    }

    double Exp::evaluate(const flo::SoundState * context) const noexcept {
        return std::exp(input.getValue(context));
    }

    double Exp2::evaluate(const flo::SoundState * context) const noexcept {
        return std::exp2(input.getValue(context));
    }

    double Exp10::evaluate(const flo::SoundState * context) const noexcept {
        constexpr auto log10 = 2.302585092994;
        return std::exp(log10 * input.getValue(context));
    }

    double Sin::evaluate(const flo::SoundState * context) const noexcept {
        return std::sin(input.getValue(context));
    }

    double Cos::evaluate(const flo::SoundState * context) const noexcept {
        return std::cos(input.getValue(context));
    }

    double Tan::evaluate(const flo::SoundState * context) const noexcept {
        return std::tan(input.getValue(context));
    }

    double Asin::evaluate(const flo::SoundState * context) const noexcept {
        return std::asin(input.getValue(context));
    }

    double Acos::evaluate(const flo::SoundState * context) const noexcept {
        return std::acos(input.getValue(context));
    }

    double Atan::evaluate(const flo::SoundState * context) const noexcept {
        return std::atan(input.getValue(context));
    }

    double Sinh::evaluate(const flo::SoundState * context) const noexcept {
        return std::sinh(input.getValue(context));
    }

    double Cosh::evaluate(const flo::SoundState * context) const noexcept {
        return std::cosh(input.getValue(context));
    }

    double Tanh::evaluate(const flo::SoundState * context) const noexcept {
        return std::tanh(input.getValue(context));
    }

    double Asinh::evaluate(const flo::SoundState * context) const noexcept {
        return std::atanh(input.getValue(context));
    }

    double Acosh::evaluate(const flo::SoundState * context) const noexcept {
        return std::acosh(input.getValue(context));
    }

    double Atanh::evaluate(const flo::SoundState * context) const noexcept {
        return std::atanh(input.getValue(context));
    }

    double Ceil::evaluate(const flo::SoundState * context) const noexcept {
        return std::ceil(input.getValue(context));
    }

    double Floor::evaluate(const flo::SoundState * context) const noexcept {
        return std::floor(input.getValue(context));
    }

    double Round::evaluate(const flo::SoundState * context) const noexcept {
        return std::round(input.getValue(context));
    }

    double Frac::evaluate(const flo::SoundState * context) const noexcept {
        const auto x = input.getValue(context);
        return x - std::floor(x);
    }

    double PlusOne::evaluate(const flo::SoundState * context) const noexcept {
        return 1.0 + input.getValue(context);
    }

    double MinusOne::evaluate(const flo::SoundState * context) const noexcept {
        return input.getValue(context) - 1.0;
    }

    double OneMinus::evaluate(const flo::SoundState * context) const noexcept {
        return 1.0 - input.getValue(context);
    }

    double Negate::evaluate(const flo::SoundState * context) const noexcept {
        return -input.getValue(context);
    }

    Reciprocal::Reciprocal()
        : UnaryFunction(1.0) {

    }

    double Reciprocal::evaluate(const flo::SoundState * context) const noexcept {
        return 1.0 / input.getValue(context);
    }

    double StdToNorm::evaluate(const flo::SoundState * context) const noexcept {
        return input.getValue(context) * 0.5 + 0.5;
    }

    double NormToStd::evaluate(const flo::SoundState * context) const noexcept {
        return input.getValue(context) * 2.0 - 1.0;
    }

    double Sigmoid::evaluate(const flo::SoundState * context) const noexcept {
        return 1.0 / (1.0 + std::exp(-input.getValue(context)));
    }

    double Min::evaluate(const flo::SoundState * context) const noexcept {
        return std::min(input1.getValue(context), input2.getValue(context));
    }

    double Max::evaluate(const flo::SoundState * context) const noexcept {
        return std::max(input1.getValue(context), input2.getValue(context));
    }

    Pow::Pow()
        : BinaryFunction(0.0, eulersConstant) {

    }

    double Pow::evaluate(const flo::SoundState * context) const noexcept {
        return std::pow(input1.getValue(context), input2.getValue(context));
    }

    LogBase::LogBase()
        : BinaryFunction(1.0, eulersConstant) {
    }

    double LogBase::evaluate(const flo::SoundState * context) const noexcept {
        return std::log(input1.getValue(context)) / std::log(input2.getValue(context));
    }

    double Hypot::evaluate(const flo::SoundState * context) const noexcept {
        return std::hypot(input1.getValue(context), input2.getValue(context));
    }

    Atan2::Atan2()
        : BinaryFunction(1.0, 0.0) {
    
    }

    double Atan2::evaluate(const flo::SoundState * context) const noexcept {
        return std::atan2(input2.getValue(context), input1.getValue(context));
    }

    double RandomUniform::evaluate(const flo::SoundState * context) const noexcept {
        const auto min = input1.getValue(context);
        const auto max = input2.getValue(context);
        const auto dist = std::uniform_real_distribution<double>(min, max);
        return dist(util::getRandomEngine());
    }

    double RandomNormal::evaluate(const flo::SoundState * context) const noexcept {
        const auto mean = input1.getValue(context);
        const auto stddev = input2.getValue(context);
        auto dist = std::normal_distribution<double>(mean, stddev);
        return dist(util::getRandomEngine());
    }

    double RoundTo::evaluate(const flo::SoundState * context) const noexcept {
        const auto a = input1.getValue(context);
        const auto b = input2.getValue(context);
        return std::round(a / b) * b;
    }

    double FloorTo::evaluate(const flo::SoundState * context) const noexcept {
        const auto a = input1.getValue(context);
        const auto b = input2.getValue(context);
        return std::floor(a / b) * b;
    }

    double CeilTo::evaluate(const flo::SoundState * context) const noexcept {
        const auto a = input1.getValue(context);
        const auto b = input2.getValue(context);
        return std::ceil(a / b) * b;
    }

    Remainder::Remainder()
        : BinaryFunction(0.0, 1.0) {

    }

    double Remainder::evaluate(const flo::SoundState * context) const noexcept {
        return std::fmod(input1.getValue(context), input2.getValue(context));
    }

    Gaussian::Gaussian()
        : input(this, 0.0)
        , center(this, 0.0)
        , width(this, 1.0)
        , amplitude(this, 1.0) {

    }

    double Gaussian::evaluate(const flo::SoundState * context) const noexcept {
        const auto a = amplitude.getValue(context);
        const auto d = input.getValue(context) - center.getValue(context);
        const auto c = width.getValue(context);
        return a * std::exp((-0.5 * d * d) / (c * c));
    }

    LinearInterpolation::LinearInterpolation()
        : start(this, 0.0)
        , end(this, 0.0)
        , fraction(this, 0.5) {

    }

    double LinearInterpolation::evaluate(const flo::SoundState * context) const noexcept {
        const auto p0 = start.getValue(context);
        const auto p1 = end.getValue(context);
        const auto t = fraction.getValue(context);
        return p0 + t * (p1 - p0);
    }

} // namespace flo

