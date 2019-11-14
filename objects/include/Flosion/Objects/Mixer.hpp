#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/SingleSoundInput.hpp>

namespace flo {

    class MixerState : public flo::SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        flo::SoundChunk buffer;
    };

    class Mixer : public flo::Realtime<flo::ControlledSoundSource<MixerState>> {
    public:

        void renderNextChunk(flo::SoundChunk& chunk, MixerState* state) override;

        SoundInput* addInput();

        void removeInput(const SoundInput*);

    private:
        std::vector<std::unique_ptr<flo::SingleSoundInput>> m_inputs;
    };


} // namespace flo
