#include <Flosion/Objects/WaveForms.hpp>

namespace flo {

    double Sine::evaluate(const flo::SoundState* context) const noexcept {
        return std::sin(input.getValue(context) * 2.0 * 3.141592654);
    }

    double Saw::evaluate(const flo::SoundState* context) const noexcept {
        const auto v = input.getValue(context);
        return 2.0 * (v - std::floor(v)) - 1.0;
    }

} // namespace flo

