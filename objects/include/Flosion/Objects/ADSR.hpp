#pragma once

#include <Flosion/Core/NumberSource.hpp>

namespace flo {

    // Attack Decay Sustain Release envelope generator
    class ADSR : public NumberSource {
    public:
        ADSR();

        // The current time
        NumberSourceInput currentTime;

        // How long, in seconds, it takes the envelope to ramp-up from 0 to 1
        NumberSourceInput attackDuration;

        // How long, in seconds, it takes the envelope to ramp-down after the
        // attack from 1 to the sustain level
        NumberSourceInput decayDuration;

        // The total length of the envelope
        NumberSourceInput totalDuration;

        // The time at which the sustain ends and release begins.
        NumberSourceInput timeOfRelease;

        // The level of
        NumberSourceInput sustainLevel;

        // NOTE: release duration is implicit, and is equal to totalDuration minus timeOfRelease.
        // Between the end of decay and the beginning of release, the envelope value is equal to
        // sustainLevel and does not change.

    private:
        double evaluate(const SoundState*) const noexcept override;
    };

} // namespace flo
