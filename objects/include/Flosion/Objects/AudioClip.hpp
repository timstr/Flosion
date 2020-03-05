#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>

#include <SFML/Audio.hpp>

namespace flo {

    class AudioClipState : public SoundState {
    public:
        using SoundState::SoundState;

        std::size_t index;

        void reset() noexcept override;
    };

    class AudioClip : public Realtime<ControlledSoundSource<AudioClipState>> {
    public:
        void loadFromFile(const std::string& path);

        sf::SoundBuffer& getSoundBuffer() noexcept;
        const sf::SoundBuffer& getSoundBuffer() const noexcept;

    private:
        void renderNextChunk(SoundChunk& chunk, AudioClipState* state) override;

        sf::SoundBuffer m_buffer;
    };

} // namespace flo
