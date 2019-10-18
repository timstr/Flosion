#pragma once

#include <Flosion/Core/Sample.hpp>

#include <array>

namespace flo {

    class SoundChunk {
    public:
        SoundChunk() noexcept;

        static constexpr size_t size = 1024;
        
        float& l(size_t i) noexcept;
        const float& l(size_t i) const noexcept;

        float& r(size_t i) noexcept;
        const float& r(size_t i) const noexcept;
        
        SampleProxy operator[](size_t i) noexcept;
        ConstSampleProxy operator[](size_t i) const noexcept;

        SampleProxy at(size_t i);
        ConstSampleProxy at(size_t i) const;

        void silence() noexcept;

    private:
        std::array<float, 2 * size> m_data;
    };

} // namespace flo
