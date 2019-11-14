#include <Flosion/Objects/Mixer.hpp>

#include <cassert>

namespace flo {

    void MixerState::reset() noexcept {
        buffer.silence();
    }

    void Mixer::renderNextChunk(flo::SoundChunk& chunk, MixerState* state){
        chunk.silence();

        // TODO: proper mixing
        for (auto& input : m_inputs){
            input->getNextChunkFor(state->buffer, this, state);
            for (size_t i = 0; i < flo::SoundChunk::size; ++i){
                chunk[i] += state->buffer[i] * 0.1f;
            }
        }
    }

    SoundInput* Mixer::addInput(){
        m_inputs.push_back(std::make_unique<flo::SingleSoundInput>(this));
        return m_inputs.back().get();
    }

    void Mixer::removeInput(const SoundInput* si){
        auto it = std::find_if(
            m_inputs.begin(),
            m_inputs.end(),
            [&](const std::unique_ptr<SingleSoundInput>& p){ return p.get() == si; }
        );
        assert(it != m_inputs.end());
        (*it)->setSource(nullptr);
        m_inputs.erase(it);
    }

} // namespace flo
