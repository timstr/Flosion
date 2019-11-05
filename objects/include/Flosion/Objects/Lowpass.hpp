#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/SingleSoundInput.hpp>

namespace flo {

    class LowpassState : public SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        Sample value {};
    };

    class Lowpass : public Realtime<ControlledSoundSource<LowpassState>> {
    public:
        Lowpass();

        SingleSoundInput input;

        SoundNumberInput cutoff;

    private:
        void renderNextChunk(SoundChunk& chunk, LowpassState* state) override;
    };

} // namespace flo
