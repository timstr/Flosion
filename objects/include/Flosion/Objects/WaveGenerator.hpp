#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>

namespace flo {

    class WaveGenerator;

    class WaveGeneratorState : public flo::ConcreteSoundState<WaveGenerator> {
    public:
        using ConcreteSoundState::ConcreteSoundState;

        void reset() noexcept override;

        double phase = 0.0;
    };

    class WaveGenerator : public flo::Realtime<flo::ControlledSoundSource<WaveGeneratorState>> {
    public:
        WaveGenerator();

        void renderNextChunk(flo::SoundChunk& chunk, WaveGeneratorState* state) override;

        class Phase : public flo::SoundNumberSource<WaveGenerator> {
        public:
            using SoundNumberSource::SoundNumberSource;

        private:
            double evaluate(const WaveGeneratorState* state, const flo::SoundState* context) const noexcept;
        } phase;

        flo::SoundNumberInput waveFunction;
        flo::SoundNumberInput frequency;

        void setPhaseSync(bool enable);

        bool getPhaseSync() const noexcept;
    
    private:
        std::atomic<bool> m_phaseSync;
    };

} // namespace flo
