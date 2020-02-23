#include <Flosion/Objects/ADSR.hpp>

namespace flo {

    ADSR::ADSR()
        : currentTime(this, 0.0)
        , attackDuration(this, 0.0)
        , decayDuration(this, 0.0)
        , totalDuration(this, 1.0)
        , timeOfRelease(this, 1.0)
        , sustainLevel(this, 1.0) {
    
    }

    double ADSR::evaluate(const SoundState* context) const noexcept {
        const auto a = std::max(0.0, attackDuration.getValue(context));
        const auto d = std::max(0.0, decayDuration.getValue(context));
        const auto s = std::clamp(sustainLevel.getValue(context), 0.0, 1.0);
        const auto l = std::max(0.0, totalDuration.getValue(context));
        const auto t = std::clamp(currentTime.getValue(context), 0.0, l);
        const auto t_rel = std::clamp(timeOfRelease.getValue(context), 0.0, l);

        if (t < 1e-6 || l < 1e-6){
            return 0.0;
        }

        double v = 0.0;

        if (t < a){
            // attack
            assert(a > 0.0);
            v = t / a;
        } else if (t - a < d){
            // decay
            assert(d > 0.0);
            v = 1.0 - (t - a) * (1.0 - s) / d;
        } else {
            // sustain
            v = s;
        }

        assert(v >= 0.0 && v <= 1.0);

        if (t >= t_rel){
            // release
            const auto rem = l - t_rel;
            assert(rem >= 0.0);
            if (rem < 1e-6){
                return 0.0;
            }
            const auto f = 1.0 - (t - t_rel) / rem;
            return f * v;
        }

        return v;
    }



} // namespace flo
