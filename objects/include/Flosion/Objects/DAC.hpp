#include <Flosion/Core/SoundResult.hpp>

#include <SFML/Audio.hpp>

namespace flo {

    class DAC : public sf::SoundStream {
    public:
        DAC();

        flo::WithCurrentTime<flo::SoundResult> soundResult;

    private:
        flo::SoundChunk m_chunk;
        std::array<sf::Int16, 2 * flo::SoundChunk::size> m_buffer;

        bool onGetData(sf::SoundStream::Chunk& out) override;

        void onSeek(sf::Time) override;
    };

} // namespace flo
