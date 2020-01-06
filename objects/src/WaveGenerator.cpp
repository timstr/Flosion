#include <Flosion/Objects/WaveGenerator.hpp>

#include <Flosion/Util/RNG.hpp>

namespace flo {

    void WaveGeneratorState::reset() noexcept {
        if (getOwner().getPhaseSync()){
            phase = 0.0;
        } else {
            auto dist = std::uniform_real_distribution<double>{0.0, 1.0};
            phase = dist(util::getRandomEngine());
        }
    }

    WaveGenerator::WaveGenerator()
        : phase(this)
        , waveFunction(this)
        , frequency(this, 250.0)
        , m_phaseSync(true) {

    }

    void WaveGenerator::renderNextChunk(flo::SoundChunk& chunk, WaveGeneratorState* state){
        for (size_t i = 0; i < flo::SoundChunk::size; ++i){
            state->adjustTime(static_cast<std::uint32_t>(i));
            float val = static_cast<float>(waveFunction.getValue(state));
            chunk.l(i) = val;
            chunk.r(i) = val;
            state->phase += frequency.getValue(state) / static_cast<double>(flo::Sample::frequency);
            state->phase -= std::floor(state->phase);
        }
    }

    double WaveGenerator::Phase::evaluate(const WaveGeneratorState* state, const flo::SoundState*) const noexcept {
        return state->phase;
    }

    void WaveGenerator::setPhaseSync(bool enable){
        m_phaseSync.store(enable, std::memory_order_relaxed);
    }

    bool WaveGenerator::getPhaseSync() const noexcept {
        return m_phaseSync.load(std::memory_order_relaxed);
    }

} // namespace flo
