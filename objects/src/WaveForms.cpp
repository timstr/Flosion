#include <Flosion/Objects/WaveForms.hpp>

#include <cmath>

namespace flo {

    double SineWave::evaluate(const flo::SoundState* context) const noexcept {
        return std::sin(input.getValue(context) * 2.0 * 3.141592654);
    }

    double SawWave::evaluate(const flo::SoundState* context) const noexcept {
        const auto v = input.getValue(context) - 0.5;
        return 2.0 * (v - std::floor(v)) - 1.0;
    }

    double SquareWave::evaluate(const flo::SoundState* context) const noexcept {
        const auto v = input.getValue(context);
        return (v - std::floor(v)) < 0.5 ? 1.0 : -1.0;
    }

    double TriangleWave::evaluate(const flo::SoundState* context) const noexcept {
        const auto v = input.getValue(context) + 0.25;
        return 1.0 - 2.0 * std::abs(1.0 - 2.0 * (v - std::floor(v)));
    }

    PulseWave::PulseWave()
        : input(this)
        , width(this, 0.5) {

    }

    double PulseWave::evaluate(const flo::SoundState* context) const noexcept {
        const auto w = std::clamp(width.getValue(context), 0.0, 1.0);
        const auto x = input.getValue(context) + 0.25;
        return std::clamp((2.0 * std::floor(x - std::floor(x - w)) - 1.0) - 2.0 * w + 1.0, -1.0, 1.0);
    }

} // namespace flo

