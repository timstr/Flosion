#include <Flosion/Core/SoundResult.hpp>

#include <SFML/Audio.hpp>

namespace flo {

    class DAC : public sf::SoundStream {
    public:
        DAC();

        flo::WithCurrentTime<flo::SoundResult> soundResult;

    private:
        flo::SoundChunk m_chunk;
        std::vector<sf::Int16> m_buffer;

        bool onGetData(sf::SoundStream::Chunk& out) override;

        void onSeek(sf::Time) override;
    };

} // namespace flo
