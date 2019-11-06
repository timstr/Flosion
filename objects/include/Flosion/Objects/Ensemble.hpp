#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/MultiSoundInput.hpp>
#include <Flosion/Util/RNG.hpp>

namespace flo {

    // TODO: sensible volume mixing

    class EnsembleInputState : public flo::SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        double randomOffset {};
    };

    class EnsembleState : public flo::SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        flo::SoundChunk buffer;
        bool init {};
    };

    class Ensemble : public flo::Realtime<flo::ControlledSoundSource<EnsembleState>> {
    public:
        // TODO: allow this to be changed using number inputs
        static const size_t numVoices = 4;
    
        Ensemble();
        ~Ensemble();
    
        void renderNextChunk(flo::SoundChunk& chunk, EnsembleState* state) override;

        flo::SoundNumberInput frequencyIn;
        flo::SoundNumberInput frequencySpread;

        class Input : public flo::MultiSoundInput<EnsembleInputState, size_t> {
        public:
            Input(Ensemble* ensemble);

            class Frequency : public flo::SoundNumberSource<Input> {    
            public:
                using SoundNumberSource::SoundNumberSource;

            private:
                double evaluate(const EnsembleInputState* state, const flo::SoundState* context) const noexcept override;
            } frequencyOut;

        private:
            Ensemble* const m_ensemble;
        } input;
    };

} // namespace flo
