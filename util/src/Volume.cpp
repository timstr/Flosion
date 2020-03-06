#include <Flosion/Util/Volume.hpp>

#include <cmath>
#include <algorithm>

namespace util {

    double volumeToAmplitude(double volumeFraction, double limit) {
        if (volumeFraction < 1e-6) {
            return 0.0;
        }
        const auto minExp = -6.0;
        return std::pow(10.0, minExp * (1.0 - std::min(volumeFraction, limit)));
    }

} // namespace util
