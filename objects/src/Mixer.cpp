#include <Flosion/Objects/Mixer.hpp>

#include <cassert>

namespace flo {

    void MixerState::reset() noexcept {
        buffer.silence();
    }

    Mixer::~Mixer() {
        while (m_inputs.size() > 0) {
            onInputRemoved.broadcast(m_inputs.front().get());
            m_inputs.erase(m_inputs.begin());
        }
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

    SingleSoundInput* Mixer::addInput(){
        auto lock = acquireLock();
        m_inputs.push_back(std::make_unique<flo::SingleSoundInput>(this));
        auto ret = m_inputs.back().get();
        onInputAdded.broadcast(ret);
        return ret;
    }

    void Mixer::removeInput(const SoundInput* si){
        auto lock = acquireLock();
        auto it = std::find_if(
            m_inputs.begin(),
            m_inputs.end(),
            [&](const std::unique_ptr<SingleSoundInput>& p){ return p.get() == si; }
        );
        assert(it != m_inputs.end());
        (*it)->setSource(nullptr);
        onInputRemoved.broadcast(it->get());
        m_inputs.erase(it);
    }

    std::vector<const SingleSoundInput*> Mixer::getInputs() const {
        std::vector<const SingleSoundInput*> ret;
        transform(
            begin(m_inputs),
            end(m_inputs),
            back_inserter(ret),
            [](const std::unique_ptr<SingleSoundInput>& up){ return up.get(); }
        );
        return ret;
    }

} // namespace flo
