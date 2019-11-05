#include <Flosion/Objects/Ensemble.hpp>

namespace flo {

    void EnsembleInputState::reset() noexcept {
        randomOffset = 1.0;
    }

    void EnsembleState::reset() noexcept {
        buffer.silence();
        init = false;
    }

    Ensemble::Ensemble()
        : frequencyIn(this, 250.0)
        , frequencySpread(this, 0.005)
        , input(this) {

        addDependency(&input);
        for (size_t k = 0; k < numVoices; ++k){
            input.addKey(k);
        }
    }

    Ensemble::~Ensemble(){
        for (size_t k = 0; k < numVoices; ++k){
            input.removeKey(k);
        }
        removeDependency(&input);
    }
    
    void Ensemble::renderNextChunk(flo::SoundChunk& chunk, EnsembleState* state){
        if (!state->init){
            auto dist = std::normal_distribution<double>(0.0, 1.0);
            for (size_t k = 0; k < numVoices; ++k){
                input.getState(this, state, k)->randomOffset = dist(util::getRandomEngine());
            }
            state->init = true;
        }
        chunk.silence();
        for (size_t k = 0; k < numVoices; ++k){
            input.getNextChunkFor(state->buffer, this, state, k);
            for (size_t i = 0; i < flo::SoundChunk::size; ++i){
                state->adjustTime(static_cast<std::uint32_t>(i));
                chunk[i] += state->buffer[i] * 0.05f;
            }
        }
    }

    Ensemble::Input::Input(Ensemble* ensemble)
        : MultiSoundInput(ensemble)
        , m_ensemble(ensemble)
        , frequencyOut(this) {
    
    }

    double Ensemble::Input::Frequency::evaluate(const EnsembleInputState* state, const flo::SoundState* context) const noexcept {
        const auto& e = this->getOwner()->m_ensemble;
        const auto f = e->frequencyIn.getValue(context);
        const auto s = e->frequencySpread.getValue(context);
        return (1.0 + state->randomOffset * s) * f;
    }

} // namespace flo
