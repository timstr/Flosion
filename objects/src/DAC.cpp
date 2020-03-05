#include <Flosion/Objects/DAC.hpp>
#include <Flosion/Core/Sample.hpp>

namespace flo {

    DAC::DAC()
        : m_buffer(2 * flo::SoundChunk::size, 0) {

        initialize(2, sampleFrequency);
    }

    bool DAC::onGetData(sf::SoundStream::Chunk& out){
        soundResult.getNextChunk(m_chunk);
        for (size_t i = 0; i < flo::SoundChunk::size; ++i){
            const auto lClamped = std::clamp(m_chunk.l(i), -1.0f, 1.0f);
            const auto rClamped = std::clamp(m_chunk.r(i), -1.0f, 1.0f);
            const auto lScaled = static_cast<float>(std::numeric_limits<std::int16_t>::max()) * lClamped;
            const auto rScaled = static_cast<float>(std::numeric_limits<std::int16_t>::max()) * rClamped;
            m_buffer[2 * i + 0] = static_cast<sf::Int16>(lScaled);
            m_buffer[2 * i + 1] = static_cast<sf::Int16>(rScaled);
        }

        out.sampleCount = 2 * flo::SoundChunk::size;
        out.samples = &m_buffer[0];

        return true;
    }

    void DAC::onSeek(sf::Time){
        // Nothing to do
    }

} // namespace flo
