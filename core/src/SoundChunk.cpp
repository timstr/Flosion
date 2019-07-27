#include <SoundChunk.hpp>

#include <cassert>

namespace flo {

    SoundChunk::SoundChunk() noexcept 
        : m_data{} {

    }

    float& SoundChunk::l(size_t i) noexcept {
        assert(i < m_data.size());
        return m_data[2 * i + 0];
    }

    float& SoundChunk::r(size_t i) noexcept {
        assert(i < m_data.size());
        return m_data[2 * i + 1];
    }

    SampleProxy SoundChunk::operator[](size_t i) noexcept {
        assert(i < m_data.size());
        return SampleProxy{&m_data[2 * i]};
    }
    ConstSampleProxy SoundChunk::operator[](size_t i) const noexcept {
        assert(i < m_data.size());
        return ConstSampleProxy{&m_data[2 * i]};
    }

    void SoundChunk::silence() noexcept {
        for (auto& v : m_data){
            v = 0.0f;
        }
    }

} // namespace flo
