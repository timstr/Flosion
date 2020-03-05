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
        ~Mixer();

        void renderNextChunk(flo::SoundChunk& chunk, MixerState* state) override;

        SingleSoundInput* addInput();

        void removeInput(const SoundInput*);

        std::vector<const SingleSoundInput*> getInputs() const;

        Signal<SingleSoundInput*> onInputAdded;
        Signal<SingleSoundInput*> onInputRemoved;

    private:
        std::vector<std::unique_ptr<SingleSoundInput>> m_inputs;
    };


} // namespace flo
