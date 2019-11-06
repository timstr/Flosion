#include <Flosion/Core/SoundQueue.hpp>

namespace flo {

    SoundQueue::SoundQueue(std::size_t nSamples){
        resize(nSamples);
    }

    void SoundQueue::resize(std::size_t nSamples){
        if (nSamples <= SoundChunk::size){
            throw std::runtime_error("Too small");
        }
        m_data.clear();
        m_data.resize(nSamples);
        m_index = 0;
    }

    std::size_t SoundQueue::size() const noexcept {
        return m_data.size();
    }

    void SoundQueue::clear(){
        for (auto& s : m_data){
            s.silence();
        }
    }

    void SoundQueue::advance(std::size_t nSamples){
        ++m_index;
        while (m_front < m_index){
            m_data[m_front].silence();
            ++m_front;
        }
    }

    void SoundQueue::advance(std::size_t nSamples, SingleSoundInput& input, const SoundNode* target, const SoundState* state){
        if (nSamples > m_data.size()){
            throw std::runtime_error("Too long");
        }
        m_index = (m_index + nSamples) % m_data.size();
        while ((m_front + SoundChunk::size) % m_data.size() <= m_index){
            SoundChunk ch;
            input.getNextChunkFor(ch, target, state);
            for (std::size_t i = 0; i < ch.size; ++i){
                m_data[m_front] = ch[i];
                ++m_front;
            }
        }
    }

    Sample& SoundQueue::operator[](std::size_t index) noexcept {
        assert(index < m_data.size());
        return m_data[(m_index + index) % m_data.size()];
    }

} // namespace flo
