#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/SingleSoundInput.hpp>

namespace flo {

    class ResamplerState : public SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept;

        flo::SoundChunk buffer;
        std::uint16_t posCoarse;
        double posFine;
        double speed;
    };

    class Resampler : public WithCurrentTime<OutOfSync<ControlledSoundSource<ResamplerState>>> {
    public:
        Resampler();

        void renderNextChunk(SoundChunk& chunk, ResamplerState* state) override;

        double getTimeSpeed(const SoundState* context) const noexcept override;

        flo::SingleSoundInput input;

        flo::SoundNumberInput timeSpeed;
    };

} // namespace flo
