#include <Flosion/Objects/AudioClip.hpp>

namespace flo {

    void AudioClipState::reset() noexcept {
        index = 0;
    }

    AudioClip::AudioClip()
        : m_looping(false) {

    }

    void AudioClip::loadFromFile(const std::string& path){
        auto l = acquireLock();
        m_buffer.loadFromFile(path);
    }

    sf::SoundBuffer& AudioClip::getSoundBuffer() noexcept {
        return m_buffer;
    }

    const sf::SoundBuffer& AudioClip::getSoundBuffer() const noexcept {
        return m_buffer;
    }

    bool AudioClip::looping() const noexcept {
        return m_looping;
    }

    void AudioClip::setLooping(bool l){
        m_looping = l;
    }

    void AudioClip::renderNextChunk(SoundChunk& chunk, AudioClipState* state){
        if (m_buffer.getSamples() == 0){
            chunk.silence();
            return;
        }

        const auto c = m_buffer.getChannelCount();
        const auto l = m_buffer.getSampleCount();
        const auto s = m_buffer.getSamples();
        const auto r = m_buffer.getSampleRate(); // TODO: use this and resample if needed
        constexpr auto k = 1.0f / static_cast<float>(std::numeric_limits<int16_t>::max());

        if (c == 1){
            for (std::size_t i = 0; i < chunk.size; ++i){
                if (m_looping && state->index >= l) {
                    state->index = 0;
                }
                if (state->index < l) {
                    chunk.l(i) = s[state->index] * k;
                    chunk.r(i) = s[state->index] * k;
                    state->index += 1;
                } else {
                    chunk[i].silence();
                }
            }
            return;
        } else if (c == 2){
            for (std::size_t i = 0; i < chunk.size; ++i){
                if (m_looping && state->index * 2 + 1 >= l) {
                    state->index = 0;
                }
                if (state->index * 2 + 1 < l) {
                    chunk.l(i) = s[state->index * 2 + 0] * k;
                    chunk.r(i) = s[state->index * 2 + 1] * k;
                    state->index += 1;
                } else {
                    chunk[i].silence();
                }
            }
            return;
        } else {
            throw std::runtime_error("What???");
        }
    }

} // namespace flo
