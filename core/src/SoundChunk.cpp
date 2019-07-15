#include <SoundChunk.hpp>

namespace flo {

    SoundChunk::SoundChunk() noexcept 
        : m_data{} {

    }

    float& SoundChunk::l(size_t i) noexcept {
        return m_data[2 * i + 0];
    }

    float& SoundChunk::r(size_t i) noexcept {
        return m_data[2 * i + 1];
    }

} // namespace flo
